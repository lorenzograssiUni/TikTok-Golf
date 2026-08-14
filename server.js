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

// Database in memoria (per testing)
const players = new Map();
const queue = [];
let currentTurnIndex = 0;

// Genera un link per un nuovo giocatore
app.post('/api/generate-link', (req, res) => {
  const { username, donationAmount } = req.body;
  
  if (!username) {
    return res.status(400).json({ error: 'Username richiesto' });
  }
  
  const token = uuidv4();
  const shots = Math.max(1, Math.floor(donationAmount || 1)); // 1€ = 1 tiro, minimo 1
  
  const player = {
    token,
    username: username || `Giocatore_${queue.length + 1}`,
    donationAmount: donationAmount || 1,
    shots,
    shotsRemaining: shots,
    score: 0,
    createdAt: new Date(),
    status: 'waiting' // waiting, playing, completed
  };
  
  players.set(token, player);
  queue.push(token);
  
  const gameUrl = `http://localhost:${PORT}/game.html?token=${token}`;
  
  res.json({
    success: true,
    player: {
      username: player.username,
      shots: player.shots,
      position: queue.length
    },
    gameUrl,
    token
  });
});

// Ottieni stato della coda
app.get('/api/queue', (req, res) => {
  const queueData = queue.map((token, index) => {
    const player = players.get(token);
    return {
      position: index + 1,
      username: player.username,
      shotsRemaining: player.shotsRemaining,
      status: index === currentTurnIndex ? 'playing' : 'waiting',
      isCurrentTurn: index === currentTurnIndex
    };
  });
  
  res.json({
    queue: queueData,
    currentTurnIndex,
    totalPlayers: queue.length
  });
});

// Ottieni classifica
app.get('/api/leaderboard', (req, res) => {
  const leaderboard = Array.from(players.values())
    .sort((a, b) => b.score - a.score)
    .map((player, index) => ({
      position: index + 1,
      username: player.username,
      score: player.score,
      shotsUsed: player.shots - player.shotsRemaining
    }));
  
  res.json({ leaderboard });
});

// Ottieni stato del giocatore
app.get('/api/player/:token', (req, res) => {
  const { token } = req.params;
  const player = players.get(token);
  
  if (!player) {
    return res.status(404).json({ error: 'Giocatore non trovato' });
  }
  
  const position = queue.indexOf(token) + 1;
  const isCurrentTurn = queue[currentTurnIndex] === token;
  
  res.json({
    player: {
      username: player.username,
      shotsRemaining: player.shotsRemaining,
      score: player.score,
      position,
      isCurrentTurn,
      status: player.status
    },
    currentTurn: players.get(queue[currentTurnIndex])?.username || null
  });
});

// Inizia turno del prossimo giocatore
app.post('/api/next-turn', (req, res) => {
  if (currentTurnIndex < queue.length - 1) {
    const previousToken = queue[currentTurnIndex];
    const previousPlayer = players.get(previousToken);
    if (previousPlayer) {
      previousPlayer.status = 'completed';
    }
    
    currentTurnIndex++;
    const currentToken = queue[currentTurnIndex];
    const currentPlayer = players.get(currentToken);
    if (currentPlayer) {
      currentPlayer.status = 'playing';
    }
    
    res.json({
      success: true,
      currentTurn: {
        username: currentPlayer.username,
        token: currentToken
      }
    });
  } else {
    res.json({
      success: false,
      message: 'Nessun altro giocatore in coda'
    });
  }
});

// Registra un tiro
app.post('/api/shot/:token', (req, res) => {
  const { token } = req.params;
  const { direction, power, score } = req.body;
  
  const player = players.get(token);
  if (!player) {
    return res.status(404).json({ error: 'Giocatore non trovato' });
  }
  
  if (player.shotsRemaining <= 0) {
    return res.status(400).json({ error: 'Nessun tiro rimanente' });
  }
  
  player.shotsRemaining--;
  player.score += score || 0;
  
  res.json({
    success: true,
    player: {
      shotsRemaining: player.shotsRemaining,
      score: player.score
    }
  });
});

// Resetta il gioco
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
