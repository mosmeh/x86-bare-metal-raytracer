#pragma once

#include "vec3.h"

typedef struct ray {
    vec3 orig, dir;
} ray;

static inline vec3 ray_at(const ray* self, double t) {
    return vec3_addv(self->orig, vec3_muls(self->dir, t));
}
