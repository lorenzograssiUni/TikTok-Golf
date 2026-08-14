#include "game.h"
#include "socket_listener.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (!game_init()) { fprintf(stderr, "Init failed\n"); return 1; }
    if (!socket_listener_init()) { printf("Warning: Socket failed\n"); }
    while (!game_should_quit()) {
        Command cmd;
        if (socket_listener_update(&cmd)) {
            switch (cmd.type) {
                case CMD_SET_DIRECTION: game_set_direction(cmd.value); break;
                case CMD_SET_POWER: game_set_power(cmd.value); break;
                case CMD_EXECUTE_SHOT: game_execute_shot(); break;
                case CMD_GET_SCORE: { int s=game_get_score(); char r[32]; sprintf(r,"%d",s); socket_listener_send_response(r); break; }
                case CMD_RESET_GAME: game_reset(); break;
                default: break;
            }
        }
        game_update(); game_render();
    }
    socket_listener_cleanup(); game_cleanup(); return 0;
}
