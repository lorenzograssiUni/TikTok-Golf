# 🎮 Guida Integrazione Open-Golf

## File creati

✅ `game-bridge.js`  
✅ `src/golf/socket_listener.h`  
✅ `src/golf/socket_listener.c`

## Modifiche necessarie

### 1. src/golf/main.c

Aggiungi `#include "socket_listener.h"` e integra socket_listener_init(), socket_listener_update(), socket_listener_cleanup().

### 2. src/golf/CMakeLists.txt

Aggiungi `socket_listener.c` ai sorgenti.

### 3. Compila

```bash
mkdir build && cd build
cmake .. && cmake --build .
```

### 4. Test

Avvia Open-Golf, poi `npm start`, poi testa con `echo "GET_SCORE" | nc localhost 9999`

## Comandi socket

SET_DIRECTION, SET_POWER, EXECUTE_SHOT, GET_SCORE, RESET_GAME
