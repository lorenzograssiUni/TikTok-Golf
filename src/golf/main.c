#include "game.h"
#include "draw.h"
#include "ui.h"
#include "golf.h"
#include "common/log.h"
#include "common/inputs.h"
#include "common/graphics.h"
#include "common/audio.h"
#include "common/thread.h"
#include "common/vec.h"

#include <sokol/gfx.h>
#include <sokol/sapp.h>
#include <sokol/saudio.h>

#include <stdio.h>
#include <stdlib.h>

static struct game *g_game;
static struct inputs *g_inputs;
static struct graphics *g_graphics;
static struct audio *g_audio;

static void
game_init(void)
{
    g_game = game_create();
    g_inputs = inputs_create();
    g_graphics = graphics_create();
    g_audio = audio_create();

    game_init_level(g_game, 0);
}

static void
game_frame(void)
{
    inputs_update(g_inputs);

    game_update(g_game, g_inputs);
    draw_frame(g_game, g_graphics, g_inputs);
    ui_frame(g_game, g_graphics, g_inputs);

    if (game_should_quit(g_game)) {
        sapp_request_quit();
    }
}

static void
game_cleanup(void)
{
    game_destroy(g_game);
    inputs_destroy(g_inputs);
    graphics_destroy(g_graphics);
    audio_destroy(g_audio);
}

SOKOL_MAIN
{
    sapp_desc desc = {
        .init_cb = game_init,
        .frame_cb = game_frame,
        .cleanup_cb = game_cleanup,
        .width = 1280,
        .height = 720,
        .window_title = "Open Golf",
        .icon.sokol_default = true,
        .high_dpi = false,
    };

    sapp_run(&desc);
}