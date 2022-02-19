#pragma once

#include "multiboot.h"
#include "vec3.h"
#include <math.h>

static inline void write_color(uint32_t* out, const vec3* color, double scale,
                               const multiboot_info_t* mbi) {
    // Divide the color by the number of samples and gamma-correct for
    // gamma=2.0.
    double r = sqrt(scale * color->x);
    double g = sqrt(scale * color->y);
    double b = sqrt(scale * color->z);

    // Write the translated [0,255] value of each color component.
    uint8_t ir = (uint8_t)(256 * clamp(r, 0, 0.999));
    uint8_t ig = (uint8_t)(256 * clamp(g, 0, 0.999));
    uint8_t ib = (uint8_t)(256 * clamp(b, 0, 0.999));
    *out = ir << mbi->framebuffer_red_field_position |
           ig << mbi->framebuffer_green_field_position |
           ib << mbi->framebuffer_blue_field_position;
}
