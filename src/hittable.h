#pragma once

#include "forward.h"
#include "vec3.h"
#include <stddef.h>

typedef struct hit_record {
    vec3 p, normal;
    double t;
    const material* mat_ptr;
    bool front_face;
} hit_record;

hittable_list* hittable_list_create(size_t capacity);

bool hittable_list_add_sphere(hittable_list*, vec3 center, double radius,
                              material);

void hittable_list_clear(hittable_list*);

bool hittable_list_hit(const hittable_list*, const ray*, double t_min,
                       double t_max, hit_record*);
