#include "sphere.h"
#include "hittable.h"
#include "ray.h"

bool sphere_hit(const sphere* self, const ray* ray, double t_min, double t_max,
                hit_record* rec) {
    vec3 oc = vec3_subv(ray->orig, self->center);
    double a = vec3_length_squared(ray->dir);
    double half_b = dot(&oc, &ray->dir);
    double c = vec3_length_squared(oc) - self->radius * self->radius;

    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
        return false;
    double sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    double root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec->t = root;
    rec->p = ray_at(ray, rec->t);
    vec3 outward_normal =
        vec3_divs(vec3_subv(rec->p, self->center), self->radius);
    rec->front_face = dot(&ray->dir, &outward_normal) < 0;
    rec->normal = rec->front_face ? outward_normal : vec3_neg(&outward_normal);
    rec->mat_ptr = &self->material;

    return true;
}
