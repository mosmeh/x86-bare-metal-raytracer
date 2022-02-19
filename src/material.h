#pragma once

#include "forward.h"
#include "vec3.h"

typedef struct lambertian {
    vec3 albedo;
} lambertian;

typedef struct metal {
    vec3 albedo;
    double fuzz;
} metal;

typedef struct dialectric {
    double ir; // Index of Refraction
} dialectric;

typedef struct material {
    union {
        lambertian lambertian;
        metal metal;
        dialectric dialectric;
    };
    enum material_type {
        MATERIAL_TYPE_LAMBERTIAN,
        MATERIAL_TYPE_METAL,
        MATERIAL_TYPE_DIALECTRIC
    } type;
} material;

material material_create_lambertian(vec3 albedo);
material material_create_metal(vec3 albedo, double fuzz);
material material_create_dialectric(double ir);

bool material_scatter(const material*, const ray* r_in, const hit_record*,
                      vec3* attenuation, ray* scattered);
