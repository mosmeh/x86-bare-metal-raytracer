#include "camera.h"
#include "ray.h"
#include "vec3.h"

camera camera_create(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov,
                     double aspect_ratio, double aperture, double focus_dist) {
    double theta = degrees_to_radians(vfov);
    double h = tan(theta / 2);
    double viewport_height = 2 * h;
    double viewport_width = aspect_ratio * viewport_height;

    vec3 w = unit_vector(vec3_subv(lookfrom, lookat));
    vec3 u = unit_vector(cross(&vup, &w));
    vec3 v = cross(&w, &u);

    vec3 origin = lookfrom;
    vec3 horizontal = vec3_muls(u, focus_dist * viewport_width);
    vec3 vertical = vec3_muls(v, focus_dist * viewport_height);
    vec3 lower_left_corner = vec3_subv(origin, vec3_divs(horizontal, 2));
    vec3_sub_assignv(&lower_left_corner, vec3_divs(vertical, 2));
    vec3_sub_assignv(&lower_left_corner, vec3_muls(w, focus_dist));

    double lens_radius = aperture / 2;

    return (camera){origin, lower_left_corner, horizontal, vertical, u, v,
                    w,      lens_radius};
}

ray camera_get_ray(const camera* self, double s, double t) {
    vec3 rd = vec3_muls(random_in_unit_disk(), self->lens_radius);
    vec3 offset = vec3_addv(vec3_muls(self->u, rd.x), vec3_muls(self->v, rd.y));

    vec3 origin = vec3_addv(self->origin, offset);
    vec3 dir =
        vec3_addv(self->lower_left_corner, vec3_muls(self->horizontal, s));
    vec3_add_assignv(&dir, vec3_muls(self->vertical, t));
    vec3_sub_assignv(&dir, self->origin);
    vec3_sub_assignv(&dir, offset);
    return (ray){origin, dir};
}
