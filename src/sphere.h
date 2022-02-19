#pragma once

#include "forward.h"
#include "material.h"
#include "vec3.h"
#include <stdbool.h>

typedef struct sphere {
    material material;
    vec3 center;
    double radius;
} sphere;

bool sphere_hit(const sphere*, const ray*, double t_min, double t_max,
                hit_record*);
