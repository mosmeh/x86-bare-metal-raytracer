#include "renderer.h"
#include "alloc.h"
#include "camera.h"
#include "color.h"
#include "hittable.h"
#include "material.h"
#include "multiboot.h"
#include "ray.h"
#include "vec3.h"
#include <stdint.h>
#include <string.h>

struct renderer {
    const multiboot_info_t* mbi;

    // Image
    uint32_t image_width, image_height;
    uint32_t max_depth;

    // World
    hittable_list* world;

    // Camera
    camera cam;

    // Render
    void* framebuffer;
    vec3* pixel_colors;
    uint32_t scanline_color;
    bool draw_scanline;
    uint32_t samples;
    uint32_t scanline_y;
};

renderer* renderer_create(const multiboot_info_t* mbi) {
    // World
    const uint32_t max_num_objects = 512;
    hittable_list* world = hittable_list_create(max_num_objects);
    if (!world)
        return NULL;

    // Camera
    vec3 lookfrom = {13, 2, 3};
    vec3 lookat = {0, 0, 0};
    vec3 vup = {0, 1, 0};
    double aspect_ratio =
        (double)mbi->framebuffer_width / mbi->framebuffer_height;
    double dist_to_focus = 10;
    double aperture = 0.1;
    camera cam = camera_create(lookfrom, lookat, vup, 20, aspect_ratio,
                               aperture, dist_to_focus);

    // Render
    void* framebuffer = (void*)(uintptr_t)mbi->framebuffer_addr;
    vec3* pixel_colors =
        alloc(mbi->framebuffer_width * mbi->framebuffer_height * sizeof(vec3));
    if (!pixel_colors)
        return NULL;

    uint32_t scanline_color = 255 << mbi->framebuffer_red_field_position |
                              127 << mbi->framebuffer_green_field_position;

    renderer* r = alloc(sizeof(renderer));
    if (!r)
        return NULL;

    r->mbi = mbi;

    r->image_width = mbi->framebuffer_width;
    r->image_height = mbi->framebuffer_height;
    r->max_depth = 50;

    r->world = world;
    r->cam = cam;

    r->framebuffer = framebuffer;
    r->pixel_colors = pixel_colors;
    r->scanline_color = scanline_color;
    r->draw_scanline = true;
    r->samples = 1;
    r->scanline_y = 0;

    renderer_clear_screen(r);
    renderer_generate_random_scene(r);

    return r;
}

void renderer_clear_screen(renderer* self) {
    self->samples = 1;
    self->scanline_y = 0;

    memset(self->framebuffer, 0,
           self->image_height * self->mbi->framebuffer_pitch);

    vec3* pixel_color = self->pixel_colors;
    for (size_t i = 0; i < self->image_width * self->image_height; ++i)
        *pixel_color++ = VEC3_ZERO;
}

void renderer_generate_random_scene(renderer* self) {
    hittable_list_clear(self->world);

    material ground_material =
        material_create_lambertian((vec3){0.5, 0.5, 0.5});
    hittable_list_add_sphere(self->world, (vec3){0, -1000, 0}, 1000,
                             ground_material);

    for (int32_t a = -11; a < 11; a++) {
        for (int32_t b = -11; b < 11; b++) {
            double choose_mat = random_double();
            vec3 center = {a + 0.9 * random_double(), 0.2,
                           b + 0.9 * random_double()};

            if (vec3_length(vec3_subv(center, (vec3){4, 0.2, 0})) > 0.9) {
                material sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    vec3 albedo = vec3_mulv(vec3_random(), vec3_random());
                    sphere_material = material_create_lambertian(albedo);
                } else if (choose_mat < 0.95) {
                    // metal
                    vec3 albedo = vec3_random_in(0.5, 1);
                    double fuzz = random_double_in(0, 0.5);
                    sphere_material = material_create_metal(albedo, fuzz);
                } else {
                    // glass
                    sphere_material = material_create_dialectric(1.5);
                }

                hittable_list_add_sphere(self->world, center, 0.2,
                                         sphere_material);
            }
        }
    }

    material material1 = material_create_dialectric(1.5);
    hittable_list_add_sphere(self->world, (vec3){0, 1, 0}, 1, material1);

    material material2 = material_create_lambertian((vec3){0.4, 0.2, 0.1});
    hittable_list_add_sphere(self->world, (vec3){-4, 1, 0}, 1, material2);

    material material3 = material_create_metal((vec3){0.7, 0.6, 0.5}, 0);
    hittable_list_add_sphere(self->world, (vec3){4, 1, 0}, 1, material3);

    hittable_list_construct_bvh(self->world);
}

static vec3 ray_color(const ray* r, const hittable_list* hittables,
                      uint32_t depth) {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth == 0) {
        return VEC3_ZERO;
    }

    hit_record rec;
    if (hittable_list_hit(hittables, r, 0.001, INFINITY, &rec)) {
        ray scattered;
        vec3 attenuation;
        if (material_scatter(rec.mat_ptr, r, &rec, &attenuation, &scattered))
            return vec3_mulv(attenuation,
                             ray_color(&scattered, hittables, depth - 1));

        return VEC3_ZERO;
    }
    vec3 unit_direction = unit_vector(r->dir);
    double t = 0.5 * (unit_direction.y + 1);
    return (vec3){1 - 0.5 * t, 1 - 0.3 * t, 1};
}

void renderer_render(renderer* self) {
    uint32_t y = self->image_height - self->scanline_y - 1;
    vec3* row_pixel_colors = &self->pixel_colors[y * self->image_width];

    {
        vec3* pixel_color = row_pixel_colors;
        for (uint32_t x = 0; x < self->image_width; ++x) {
            double u = (x + random_double()) / (self->image_width - 1);
            double v = (y + random_double()) / (self->image_height - 1);
            ray ray = camera_get_ray(&self->cam, u, v);
            vec3 color = ray_color(&ray, self->world, self->max_depth);
            vec3_add_assignv(pixel_color++, color);
        }
    }
    {
        double scale = 1.0 / self->samples;
        uint32_t* pixel =
            (uint32_t*)(self->framebuffer +
                        self->scanline_y * self->mbi->framebuffer_pitch);
        vec3* pixel_color = row_pixel_colors;
        for (uint32_t x = 0; x < self->image_width; ++x)
            write_color(pixel++, pixel_color++, scale, self->mbi);
    }

    if (++self->scanline_y >= self->image_height) {
        self->scanline_y = 0;
        ++self->samples;
    }

    if (self->draw_scanline) {
        uint32_t* pixel =
            (uint32_t*)(self->framebuffer +
                        self->scanline_y * self->mbi->framebuffer_pitch);
        for (uint32_t x = 0; x < self->image_width; ++x)
            *pixel++ = self->scanline_color;
    }
}

void renderer_toggle_draw_scanline(renderer* self) {
    self->draw_scanline = !self->draw_scanline;
}
