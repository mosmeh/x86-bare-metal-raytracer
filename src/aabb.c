#include "aabb.h"
#include "ray.h"

static bool hit_axis(double orig, double dir, double minimum, double maximum,
                     double* t_min, double* t_max) {
    double inv_d = 1 / dir;
    double t0 = (minimum - orig) * inv_d;
    double t1 = (maximum - orig) * inv_d;
    if (inv_d < 0) {
        double tmp = t0;
        t0 = t1;
        t1 = tmp;
    }
    *t_min = t0 > *t_min ? t0 : *t_min;
    *t_max = t1 < *t_max ? t1 : *t_max;
    return *t_max > *t_min;
}

bool aabb_hit(const aabb* self, const ray* r, double t_min, double t_max) {
    if (!hit_axis(r->orig.x, r->dir.x, self->minimum.x, self->maximum.x, &t_min,
                  &t_max))
        return false;
    if (!hit_axis(r->orig.y, r->dir.y, self->minimum.y, self->maximum.y, &t_min,
                  &t_max))
        return false;
    if (!hit_axis(r->orig.z, r->dir.z, self->minimum.z, self->maximum.z, &t_min,
                  &t_max))
        return false;
    return true;
}

aabb surrounding_box(const aabb* box0, const aabb* box1) {
    vec3 small = (vec3){fmin(box0->minimum.x, box1->minimum.x),
                        fmin(box0->minimum.y, box1->minimum.y),
                        fmin(box0->minimum.z, box1->minimum.z)};

    vec3 big = (vec3){fmax(box0->maximum.x, box1->maximum.x),
                      fmax(box0->maximum.y, box1->maximum.y),
                      fmax(box0->maximum.z, box1->maximum.z)};

    return (aabb){small, big};
}
