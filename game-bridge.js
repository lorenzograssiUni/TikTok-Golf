import net from 'net';

const GAME_HOST = '127.0.0.1';
const GAME_PORT = 9999;

let socket = null;
let connected = false;
let commandQueue = [];

function connect() {
  return new Promise((resolve, reject) => {
    socket = net.createConnection({ host: GAME_HOST, port: GAME_PORT }, () => {
      console.log('✅ Connesso a Open-Golf');
      connected = true;
      resolve();
    });
    socket.on('error', (err) => {
      console.error('❌ Errore connessione Open-Golf:', err.message);
      connected = false;
      reject(err);
    });
    socket.on('close', () => {
      console.log('⚠️  Disconnesso da Open-Golf');
      connected = false;
    });
    socket.on('data', (data) => {
      const response = data.toString().trim();
      console.log('📥 Risposta da Open-Golf:', response);
      if (commandQueue.length > 0) {
        const { resolve } = commandQueue.shift();
        resolve(response);
      }
    });
  });
}

function sendCommand(command) {
  return new Promise((resolve, reject) => {
    if (!connected || !socket) return reject(new Error('Non connesso a Open-Golf'));
    console.log('📤 Invio comando:', command);
    socket.write(command + '\n');
    commandQueue.push({ resolve, reject, timestamp: Date.now() });
    setTimeout(() => {
      if (commandQueue.length > 0) {
        commandQueue.shift();
        reject(new Error('Timeout comando: ' + command));
      }
    }, 5000);
  });
}

export const gameBridge = {
  connect,
  async setDirection(value) {
    const normalized = Math.max(0, Math.min(100, value));
    return await sendCommand(`SET_DIRECTION ${normalized}`);
  },
  async setPower(value) {
    const normalized = Math.max(0, Math.min(100, value));
    return await sendCommand(`SET_POWER ${normalized}`);
  },
  async executeShot() {
    return await sendCommand('EXECUTE_SHOT');
  },
  async getScore() {
    const response = await sendCommand('GET_SCORE');
    return parseInt(response, 10);
  },
  async resetGame() {
    return await sendCommand('RESET_GAME');
  },
  isConnected() {
    return connected;
  }
};

export default gameBridge;

if (process.argv[1]?.endsWith('game-bridge.js')) {
  console.log('Testing game-bridge...');
  gameBridge.connect()
    .then(() => gameBridge.setDirection(50))
    .then(() => gameBridge.setPower(75))
    .then(() => gameBridge.executeShot())
    .then(() => gameBridge.getScore())
    .then(score => console.log('Punteggio:', score))
    .catch(err => console.error('Errore:', err.message));
}
