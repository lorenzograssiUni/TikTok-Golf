#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <stdbool.h>

#define SOCKET_PORT 9999
#define BUFFER_SIZE 256

typedef enum {
    CMD_NONE,
    CMD_SET_DIRECTION,
    CMD_SET_POWER,
    CMD_EXECUTE_SHOT,
    CMD_GET_SCORE,
    CMD_RESET_GAME
} CommandType;

typedef struct {
    CommandType type;
    int value;
} Command;

bool socket_listener_init(void);
bool socket_listener_update(Command* cmd);
void socket_listener_send_response(const char* response);
void socket_listener_cleanup(void);
bool socket_listener_is_connected(void);

#endif
