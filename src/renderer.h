#pragma once

#include "forward.h"

renderer* renderer_create(const multiboot_info_t*);
void renderer_clear_screen(renderer*);
void renderer_generate_random_scene(renderer*);
void renderer_render(renderer*);
void renderer_toggle_draw_scanline(renderer*);
