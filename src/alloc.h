#pragma once

#include "forward.h"
#include <stdbool.h>
#include <stddef.h>

bool alloc_init(const multiboot_info_t*);
void* alloc(size_t size);
