#pragma once

#include "forward.h"
#include <stdbool.h>
#include <stddef.h>

bvh_node* bvh_create(const sphere* objects, size_t num_objects);
bool bvh_hit(const bvh_node*, const ray*, double t_min, double t_max,
             hit_record*);
