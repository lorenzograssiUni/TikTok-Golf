#include "game.h"
#include "draw.h"
#include "ui.h"
#include "golf.h"
#include "common/log.h"
#include "common/inputs.h"
#include "common/graphics.h"
#include "common/audio.h"
#include "socket_listener.h"

#include <sokol/sapp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct game *g_game;
static struct inputs *g_inputs;
static struct graphics *g_graphics;
static struct audio *g_audio;

static void
init_cb(void)
{
    if (socket_listener_init() != 0) {
        log_error("Failed to initialize socket listener");
    }

    g_game = game_create();
    g_inputs = inputs_create();
    g_graphics = graphics_create();
    g_audio = audio_create();

    game_init_level(g_game, 0);
}

static void
frame_cb(void)
{
    inputs_update(g_inputs);

    game_update(g_game, g_inputs);
    draw_frame(g_game, g_graphics, g_inputs);
    ui_frame(g_game, g_graphics, g_inputs);

    char buf[256];
    int n = socket_listener_poll(buf, sizeof(buf));
    if (n > 0) {
        buf[n] = '\0';
        if (strcmp(buf, "RESET") == 0) {
            game_init_level(g_game, game_get_current_level(g_game));
        } else if (strcmp(buf, "NEXT") == 0) {
            int cur = game_get_current_level(g_game);
            game_init_level(g_game, cur + 1);
        } else if (strcmp(buf, "QUIT") == 0) {
            sapp_request_quit();
        }
    }

    if (game_should_quit(g_game)) {
        sapp_request_quit();
    }
}

static void
cleanup_cb(void)
{
    socket_listener_cleanup();

    game_destroy(g_game);
    inputs_destroy(g_inputs);
    graphics_destroy(g_graphics);
    audio_destroy(g_audio);
}

int main(void)
{
    sapp_desc desc = {
        .init_cb = init_cb,
        .frame_cb = frame_cb,
        .cleanup_cb = cleanup_cb,
        .width = 1280,
        .height = 720,
        .window_title = "Open Golf",
        .icon.sokol_default = true,
        .high_dpi = false,
    };

    return sapp_run(&desc);
}