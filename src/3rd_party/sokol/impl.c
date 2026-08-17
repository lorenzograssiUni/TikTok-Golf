#define SOKOL_IMPL
#define SOKOL_GLCORE33

#include "sokol/gfx.h"
#include "sokol/sapp.h"
#include "sokol/saudio.h"
#include "sokol/glue.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"
#include "sokol/imgui.h"

#if !defined(_WIN32)
SOKOL_API_IMPL int main(void) {
    return sokol_main(0, NULL);
}
#endif