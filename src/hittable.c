#include "hittable.h"
#include "aabb.h"
#include "alloc.h"
#include "bvh.h"
#include "sphere.h"

struct hittable_list {
    size_t capacity, size;
    sphere* spheres;
    const bvh_node* bvh;
};

hittable_list* hittable_list_create(size_t capacity) {
    hittable_list* hittables = alloc(sizeof(hittable_list));
    if (!hittables)
        return NULL;
    hittables->capacity = capacity;
    hittables->size = 0;
    hittables->bvh = NULL;
    hittables->spheres = alloc(capacity * sizeof(sphere));
    if (!hittables->spheres)
        return NULL;
    return hittables;
}

bool hittable_list_add_sphere(hittable_list* self, vec3 center, double radius,
                              material material) {
    if (self->size >= self->capacity)
        return false;

    self->spheres[self->size++] =
        (sphere){.center = center, .radius = radius, .material = material};
    return true;
}

void hittable_list_clear(hittable_list* self) {
    self->size = 0;
    self->bvh = NULL;
}

void hittable_list_construct_bvh(hittable_list* self) {
    self->bvh = bvh_create(self->spheres, self->size);
}

bool hittable_list_hit(const hittable_list* self, const ray* r, double t_min,
                       double t_max, hit_record* rec) {
    if (self->bvh)
        return bvh_hit(self->bvh, r, t_min, t_max, rec);

    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;

    for (uint32_t i = 0; i < self->size; ++i) {
        if (sphere_hit(&self->spheres[i], r, t_min, closest_so_far,
                       &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }

    return hit_anything;
}
