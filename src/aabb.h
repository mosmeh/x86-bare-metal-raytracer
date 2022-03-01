#pragma once

#include "forward.h"
#include "vec3.h"

typedef struct aabb {
    vec3 minimum, maximum;
} aabb;

bool aabb_hit(const aabb*, const ray*, double t_min, double t_max);

aabb surrounding_box(const aabb* box0, const aabb* box1);
