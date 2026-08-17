import express from 'express';
import { v4 as uuidv4 } from 'uuid';
import cors from 'cors';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const PORT = 3000;

app.use(cors());
app.use(express.json());
app.use(express.static(join(__dirname, 'public')));

const players = new Map();
const queue = [];
let currentTurnIndex = 0;

app.post('/api/generate-link', (req, res) => {
  const usernameValue = typeof req.body?.username === 'string' ? req.body.username.trim() : '';
  const donationValue = Number(req.body?.donationAmount);
  if (!usernameValue) return res.status(400).json({ error: 'Username richiesto' });
  if (!Number.isFinite(donationValue) || donationValue < 1) return res.status(400).json({ error: 'Donazione non valida' });
  const token = uuidv4();
  const shots = Math.max(1, Math.floor(donationValue));
  const player = { token, username: usernameValue.slice(0, 50), donationAmount: donationValue, shots, shotsRemaining: shots, score: 0, createdAt: new Date(), status: queue.length === 0 ? 'playing' : 'waiting' };
  players.set(token, player);
  queue.push(token);
  const gameUrl = `http://localhost:${PORT}/game.html?token=${token}`;
  res.json({ success: true, player: { username: player.username, shots: player.shots, position: queue.length }, gameUrl, token });
});

app.get('/api/queue', (req, res) => {
  const queueData = queue.map((token, index) => {
    const player = players.get(token);
    return { position: index + 1, username: player.username, shotsRemaining: player.shotsRemaining, status: player.status, isCurrentTurn: index === currentTurnIndex && player.shotsRemaining > 0 };
  });
  res.json({ queue: queueData, currentTurnIndex, totalPlayers: queue.length });
});

app.get('/api/leaderboard', (req, res) => {
  const leaderboard = Array.from(players.values()).sort((a, b) => b.score - a.score).map((player, index) => ({ position: index + 1, username: player.username, score: player.score, shotsUsed: player.shots - player.shotsRemaining }));
  res.json({ leaderboard });
});

app.get('/api/player/:token', (req, res) => {
  const { token } = req.params;
  const player = players.get(token);
  if (!player) return res.status(404).json({ error: 'Giocatore non trovato' });
  const position = queue.indexOf(token) + 1;
  
  // Auto-advance: se il giocatore corrente ha finito i tiri, passa al prossimo
  if (queue[currentTurnIndex]) {
    const currentPlayer = players.get(queue[currentTurnIndex]);
    if (currentPlayer && currentPlayer.shotsRemaining <= 0 && currentTurnIndex < queue.length - 1) {
      currentPlayer.status = 'completed';
      currentTurnIndex++;
      const nextPlayer = players.get(queue[currentTurnIndex]);
      if (nextPlayer) nextPlayer.status = 'playing';
    }
  }
  
  res.json({ player: { username: player.username, shotsRemaining: player.shotsRemaining, score: player.score, position, isCurrentTurn: queue[currentTurnIndex] === token, status: player.status }, currentTurn: players.get(queue[currentTurnIndex])?.username || null });
});

app.post('/api/next-turn', (req, res) => {
  if (currentTurnIndex < queue.length - 1) {
    const previousToken = queue[currentTurnIndex];
    const previousPlayer = players.get(previousToken);
    if (previousPlayer) previousPlayer.status = 'completed';
    currentTurnIndex++;
    const currentToken = queue[currentTurnIndex];
    const currentPlayer = players.get(currentToken);
    if (currentPlayer) currentPlayer.status = 'playing';
    res.json({ success: true, currentTurn: { username: currentPlayer.username, token: currentToken } });
  } else {
    res.json({ success: false, message: 'Nessun altro giocatore in coda' });
  }
});

app.post('/api/shot/:token', (req, res) => {
  const { token } = req.params;
  const { direction, power, score } = req.body || {};
  const player = players.get(token);
  if (!player) return res.status(404).json({ error: 'Giocatore non trovato' });
  if (queue[currentTurnIndex] !== token) return res.status(403).json({ error: 'Non è il turno di questo giocatore' });
  if (player.shotsRemaining <= 0) return res.status(400).json({ error: 'Nessun tiro rimanente' });
  const directionValue = Number(direction);
  const powerValue = Number(power);
  const scoreValue = Number(score);
  if (![directionValue, powerValue, scoreValue].every(Number.isFinite)) return res.status(400).json({ error: 'Parametri del tiro non validi' });
  if (directionValue < 0 || directionValue > 100 || powerValue < 0 || powerValue > 100 || scoreValue < 0 || scoreValue > 100) return res.status(400).json({ error: 'Parametri del tiro fuori intervallo' });
  player.shotsRemaining--;
  player.score += Math.round(scoreValue);
  
  // Se ha finito i tiri e ci sono altri giocatori, passa automaticamente
  if (player.shotsRemaining <= 0 && currentTurnIndex < queue.length - 1) {
    player.status = 'completed';
    currentTurnIndex++;
    const nextPlayer = players.get(queue[currentTurnIndex]);
    if (nextPlayer) nextPlayer.status = 'playing';
  }
  
  res.json({ success: true, player: { shotsRemaining: player.shotsRemaining, score: player.score } });
});

app.post('/api/reset', (req, res) => {
  players.clear();
  queue.length = 0;
  currentTurnIndex = 0;
  res.json({ success: true, message: 'Gioco resettato' });
});

app.listen(PORT, () => {
  console.log(`Server avviato su http://localhost:${PORT}`);
  console.log(`Admin: http://localhost:${PORT}/admin.html`);
  console.log(`Game: http://localhost:${PORT}/game.html?token=XXX`);
});
