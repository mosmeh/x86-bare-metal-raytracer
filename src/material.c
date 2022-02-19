#include "material.h"
#include "hittable.h"
#include "ray.h"
#include <stddef.h>

material material_create_lambertian(vec3 albedo) {
    return (material){.type = MATERIAL_TYPE_LAMBERTIAN, .lambertian = {albedo}};
}

static bool lambertian_scatter(const lambertian* self, const ray* r_in,
                               const hit_record* rec, vec3* attenuation,
                               ray* scattered) {
    (void)r_in;

    vec3 scatter_direction = vec3_addv(rec->normal, random_unit_vector());

    // Catch degenerate scatter direction
    if (vec3_near_zero(&scatter_direction))
        scatter_direction = rec->normal;

    *scattered = (ray){rec->p, scatter_direction};
    *attenuation = self->albedo;
    return true;
}

material material_create_metal(vec3 albedo, double fuzz) {
    return (material){.type = MATERIAL_TYPE_METAL,
                      .metal = {.albedo = albedo, .fuzz = fuzz < 1 ? fuzz : 1}};
}

static vec3 reflect(const vec3* v, const vec3* n) {
    return vec3_subv(*v, vec3_muls(*n, 2 * dot(v, n)));
}

static bool metal_scatter(const metal* self, const ray* r_in,
                          const hit_record* rec, vec3* attenuation,
                          ray* scattered) {
    vec3 normalized_r_in = unit_vector(r_in->dir);
    vec3 reflected = reflect(&normalized_r_in, &rec->normal);
    *scattered = (ray){
        rec->p,
        vec3_addv(reflected, vec3_muls(random_in_unit_sphere(), self->fuzz))};
    *attenuation = self->albedo;
    return dot(&scattered->dir, &rec->normal) > 0;
}

static vec3 refract(const vec3* uv, const vec3* n, double etai_over_etat) {
    double cos_theta = fmin(-dot(uv, n), 1);
    vec3 r_out_perp =
        vec3_muls(vec3_addv(*uv, vec3_muls(*n, cos_theta)), etai_over_etat);
    vec3 r_out_parallel =
        vec3_muls(*n, -sqrt(fabs(1 - vec3_length_squared(r_out_perp))));
    return vec3_addv(r_out_perp, r_out_parallel);
}

static double reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    double r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

material material_create_dialectric(double ir) {
    return (material){.type = MATERIAL_TYPE_DIALECTRIC, .dialectric = {ir}};
}

static bool dialectric_scatter(const dialectric* self, const ray* r_in,
                               const hit_record* rec, vec3* attenuation,
                               ray* scattered) {
    *attenuation = (vec3){1, 1, 1};
    double refraction_ratio = rec->front_face ? (1 / self->ir) : self->ir;

    vec3 unit_direction = unit_vector(r_in->dir);
    double cos_theta = fmin(-dot(&unit_direction, &rec->normal), 1);
    double sin_theta = sqrt(1 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1;
    vec3 direction;

    if (cannot_refract ||
        reflectance(cos_theta, refraction_ratio) > random_double())
        direction = reflect(&unit_direction, &rec->normal);
    else
        direction = refract(&unit_direction, &rec->normal, refraction_ratio);

    *scattered = (ray){rec->p, direction};
    return true;
}

bool material_scatter(const material* self, const ray* r_in,
                      const hit_record* rec, vec3* attenuation,
                      ray* scattered) {
    switch (self->type) {
    case MATERIAL_TYPE_LAMBERTIAN:
        return lambertian_scatter(&self->lambertian, r_in, rec, attenuation,
                                  scattered);
    case MATERIAL_TYPE_METAL:
        return metal_scatter(&self->metal, r_in, rec, attenuation, scattered);
    case MATERIAL_TYPE_DIALECTRIC:
        return dialectric_scatter(&self->dialectric, r_in, rec, attenuation,
                                  scattered);
    }
    __builtin_unreachable();
}
