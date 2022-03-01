#pragma once

#include "common.h"
#include <math.h>
#include <stdbool.h>

typedef struct vec3 {
    double x, y, z;
} vec3;

static const vec3 VEC3_ZERO = {};

static inline vec3 vec3_addv(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline vec3 vec3_adds(vec3 a, double b) {
    return (vec3){a.x + b, a.y + b, a.z + b};
}

static inline void vec3_add_assignv(vec3* a, vec3 b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

static inline vec3 vec3_subv(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline vec3 vec3_subs(vec3 a, double b) {
    return (vec3){a.x - b, a.y - b, a.z - b};
}

static inline void vec3_sub_assignv(vec3* a, vec3 b) {
    a->x -= b.x;
    a->y -= b.y;
    a->z -= b.z;
}

static inline vec3 vec3_mulv(vec3 a, vec3 b) {
    return (vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline vec3 vec3_muls(vec3 a, double b) {
    return (vec3){a.x * b, a.y * b, a.z * b};
}

static inline vec3 vec3_divs(vec3 a, double b) {
    return (vec3){a.x / b, a.y / b, a.z / b};
}

static inline vec3 vec3_neg(const vec3* v) {
    return (vec3){-v->x, -v->y, -v->z};
}

static inline double vec3_length_squared(vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline double vec3_length(vec3 v) {
    return sqrt(vec3_length_squared(v));
}

static inline vec3 vec3_random(void) {
    return (vec3){random_double(), random_double(), random_double()};
}

static inline vec3 vec3_random_in(double min, double max) {
    return (vec3){random_double_in(min, max), random_double_in(min, max),
                  random_double_in(min, max)};
}

static inline bool vec3_near_zero(const vec3* v) {
    // Return true if the vector is close to zero in all dimensions.
    const double s = 1e-8;
    return (fabs(v->x) < s) && (fabs(v->y) < s) && (fabs(v->z) < s);
}

static inline double dot(const vec3* a, const vec3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

static inline vec3 cross(const vec3* a, const vec3* b) {
    return (vec3){a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z,
                  a->x * b->y - a->y * b->x};
}

static inline vec3 unit_vector(vec3 v) { return vec3_divs(v, vec3_length(v)); }

static inline vec3 random_in_unit_sphere(void) {
    while (true) {
        vec3 p = vec3_random_in(-1, 1);
        if (vec3_length_squared(p) >= 1)
            continue;
        return p;
    }
}

static inline vec3 random_unit_vector(void) {
    return unit_vector(random_in_unit_sphere());
}

static inline vec3 random_in_unit_disk(void) {
    while (true) {
        vec3 p = {random_double_in(-1, 1), random_double_in(-1, 1), 0};
        if (vec3_length_squared(p) >= 1)
            continue;
        return p;
    }
}
