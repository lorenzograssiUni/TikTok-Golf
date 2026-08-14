#include "game.h"
#include "draw.h"
#include "ui.h"
#include "golf.h"
#include "common/log.h"
#include "common/inputs.h"
#include "common/graphics.h"
#include "common/audio.h"

#include <sokol/sapp.h>

#include <stdio.h>
#include <stdlib.h>

static void
golf_init_cb(void)
{
    golf_init();
}

static void
golf_frame_cb(void)
{
    golf_frame();
}

static void
golf_cleanup_cb(void)
{
    golf_cleanup();
}

SOKOL_MAIN
{
    sapp_desc desc = {
        .init_cb = golf_init_cb,
        .frame_cb = golf_frame_cb,
        .cleanup_cb = golf_cleanup_cb,
        .width = 1280,
        .height = 720,
        .window_title = "Open Golf",
        .icon.sokol_default = true,
        .high_dpi = false,
    };

    sapp_run(&desc);
}