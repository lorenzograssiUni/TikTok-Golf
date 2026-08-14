# 🎯 TikTok Golf

Progetto Open Golf integrato con sistema di donazioni per TikTok Live. Gli utenti donano e ricevono un link per giocare, con coda e classifica in tempo reale.

## 📁 Struttura

```
TikTok-Golf/
├── src/              # Codice C di Open-Golf
├── public/           # Frontend web
│   ├── admin.html    # Pannello admin
│   ├── game.html     # Pagina giocatore
│   └── style.css     # Stili CSS
├── server.js         # Backend Node.js
├── package.json      # Dipendenze Node.js
└── README.md         # Questa documentazione
```

## 🚀 Installazione

### 1. Installa Node.js

Scarica Node.js da [nodejs.org](https://nodejs.org/)

### 2. Installa le dipendenze

```bash
cd TikTok-Golf
npm install
```

### 3. Avvia il server

```bash
npm start
```

Il server si avvierà¹° su `http://localhost:3000`

## 📖 Utilizzo

### Pannello Admin

1. Apri `http://localhost:3000/admin.html`
2. Inserisci username TikTok e importo donazione
3. Clicca "Genera Link"
4. Copia il link e condividilo con l'utente

### Pagina Giocatore

1. L'utente apre il link (es. `http://localhost:3000/game.html?token=XXX`)
2. Vede posizione in coda e tiri disponibili
3. Quando è il suo turno:
   - Ferma direzione (indicatore destra/sinistra)
   - Ferma forza (barra su/giÙ¹⁰)
   - Esegue il tiro

### Gestione Coda

- **Prossimo Turno**: Passa al giocatore successivo
- **Classifica**: Giocatori ordinati per punteggio
- **Resetta**: Cancella tutto e ricomincia

## 🎮 Modalità¹° di Gioco

1. **Direzione**: Si muove automaticamente. Clicca "FERMA DIREZIONE".
2. **Forza**: Barra che sale/scende. Clicca "FERMA FORZA".
3. **Tiro**: Punteggio = media tra direzione e forza.

## 🔧 API

- `POST /api/generate-link` - Crea giocatore
- `GET /api/queue` - Ottieni coda
- `GET /api/leaderboard` - Ottieni classifica
- `GET /api/player/:token` - Stato giocatore
- `POST /api/next-turn` - Prossimo turno
- `POST /api/shot/:token` - Registra tiro
- `POST /api/reset` - Resetta gioco

## 📝 Note

- Progetto di testing per simulare donazioni TikTok
- Database in memoria (si resetta riavviando)
- Per TikTok reale: implementare webhook donazioni

## 🛠️ Prossimi Passi

1. Integrare Open-Golf (C) con backend
2. Aggiungere WebSocket per real-time
3. Implementare webhook TikTok
4. Creare overlay per OBS

## 👨‍💻 Autore

lorenzograssiUni

## 📄 License

MIT
