#pragma once

#include "forward.h"
#include "vec3.h"

typedef struct camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal, vertical;
    vec3 u, v, w;
    double lens_radius;
} camera;

camera camera_create(vec3 lookfrom, vec3 lookat, vec3 vup,
                     double vfov, // vertical field-of-view in degrees
                     double aspect_ratio, double aperture, double focus_dist);

ray camera_get_ray(const camera*, double s, double t);
