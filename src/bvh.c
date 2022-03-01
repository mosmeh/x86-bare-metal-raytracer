#include "bvh.h"
#include "aabb.h"
#include "alloc.h"
#include "hittable.h"
#include "sphere.h"

typedef struct inner_node {
    aabb box;
    bvh_node* left;
    bvh_node* right;
} inner_node;

typedef struct bvh_node {
    union {
        inner_node inner;
        const sphere* leaf;
    };
    bool is_leaf;
} bvh_node;

typedef struct bvh_node_list {
    bvh_node* next_node;
} bvh_node_list;

static aabb bvh_node_bounding_box(const bvh_node* self) {
    if (self->is_leaf)
        return sphere_bounding_box(self->leaf);
    return self->inner.box;
}

static bvh_node* bvh_node_list_add_node(bvh_node_list* self) {
    return self->next_node++;
}

typedef const sphere* sphere_ptr;
typedef bool (*compare_fn)(sphere_ptr, sphere_ptr);

static bool box_x_compare(sphere_ptr a, sphere_ptr b) {
    return sphere_bounding_box(a).minimum.x < sphere_bounding_box(b).minimum.x;
}
static bool box_y_compare(sphere_ptr a, sphere_ptr b) {
    return sphere_bounding_box(a).minimum.y < sphere_bounding_box(b).minimum.y;
}
static bool box_z_compare(sphere_ptr a, sphere_ptr b) {
    return sphere_bounding_box(a).minimum.z < sphere_bounding_box(b).minimum.z;
}

static void swap(sphere_ptr* a, sphere_ptr* b) {
    sphere_ptr tmp = *a;
    *a = *b;
    *b = tmp;
}

static void partition_at_index(sphere_ptr* start, sphere_ptr* end, size_t index,
                               compare_fn less_than) {
    while (true) {
        size_t size = end - start;
        if (size <= 1)
            return;

        size_t pivot_idx = size / 2;
        swap(start + pivot_idx, start);

        sphere_ptr pivot = *start;

        size_t i = 0;
        for (size_t j = 1; j < size; ++j) {
            if (less_than(*(start + j), pivot)) {
                swap(start + j, start + i + 1);
                ++i;
            }
        }

        swap(start, start + i);
        if (i == index)
            return;
        if (i > index) {
            end = start + i;
        } else {
            start += i + 1;
            index -= i + 1;
        }
    }
}

static bvh_node* create_node(bvh_node_list* nodes, sphere_ptr* objects,
                             size_t num_objects) {
    if (num_objects == 1) {
        bvh_node* node = bvh_node_list_add_node(nodes);
        node->is_leaf = true;
        node->leaf = *objects;
        return node;
    }

    int axis = random_int(0, 2);
    compare_fn comparator = (axis == 0)   ? box_x_compare
                            : (axis == 1) ? box_y_compare
                                          : box_z_compare;

    size_t mid = num_objects / 2;
    partition_at_index(objects, objects + num_objects, mid, comparator);

    bvh_node* left = create_node(nodes, objects, mid);
    bvh_node* right = create_node(nodes, objects + mid, num_objects - mid);

    aabb box_left = bvh_node_bounding_box(left);
    aabb box_right = bvh_node_bounding_box(right);
    aabb box = surrounding_box(&box_left, &box_right);

    bvh_node* node = bvh_node_list_add_node(nodes);
    node->is_leaf = false;
    node->inner = (inner_node){.left = left, .right = right, .box = box};
    return node;
}

bvh_node* bvh_create(const sphere* objects, size_t num_objects) {
    if (num_objects == 0)
        return NULL;

    sphere_ptr* ptrs = alloc(num_objects * sizeof(sphere_ptr));
    if (!ptrs)
        return NULL;
    sphere_ptr* ptr = ptrs;
    sphere_ptr obj = objects;
    for (size_t i = 0; i < num_objects; ++i)
        *ptr++ = obj++;

    size_t capacity = 2 * num_objects - 1;
    bvh_node_list nodes = {.next_node = alloc(capacity * sizeof(bvh_node))};
    if (!nodes.next_node)
        return NULL;

    return create_node(&nodes, ptrs, num_objects);
}

static bool inner_node_hit(const inner_node* self, const ray* r, double t_min,
                           double t_max, hit_record* rec) {
    if (!aabb_hit(&self->box, r, t_min, t_max))
        return false;

    bool hit_left = bvh_hit(self->left, r, t_min, t_max, rec);
    bool hit_right =
        bvh_hit(self->right, r, t_min, hit_left ? rec->t : t_max, rec);

    return hit_left || hit_right;
}

bool bvh_hit(const bvh_node* self, const ray* r, double t_min, double t_max,
             hit_record* rec) {
    if (self->is_leaf)
        return sphere_hit(self->leaf, r, t_min, t_max, rec);
    return inner_node_hit(&self->inner, r, t_min, t_max, rec);
}
