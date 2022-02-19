#include "alloc.h"
#include "multiboot.h"
#include <stddef.h>
#include <stdint.h>

extern unsigned char end[];

static uintptr_t heap_ptr = 0;
static uintptr_t heap_end = 0;

static uintptr_t round_up(uintptr_t x) {
    const size_t align = _Alignof(max_align_t);
    return (x + (align - 1)) & ~(align - 1);
}

bool alloc_init(const multiboot_info_t* mbi) {
    if (!(mbi->flags & MULTIBOOT_INFO_MEM_MAP)) {
        heap_ptr = round_up((uintptr_t)end);
        heap_end = mbi->mem_upper * 0x400 + 0x100000;
        return heap_ptr < heap_end;
    }

    uint32_t num_entries = mbi->mmap_length / sizeof(multiboot_memory_map_t);
    const multiboot_memory_map_t* entry =
        (const multiboot_memory_map_t*)mbi->mmap_addr;
    size_t max_len = 0;

    for (uint32_t i = 0; i < num_entries; ++i, ++entry) {
        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        uintptr_t entry_start = entry->addr;
        uintptr_t entry_end = entry->addr + entry->len;

        if (entry_start < (uintptr_t)end)
            entry_start = (uintptr_t)end;
        entry_start = round_up(entry_start);

        if (entry_start >= entry_end)
            continue;

        size_t entry_len = entry_end - entry_start;
        if (entry_len > max_len) {
            max_len = entry_len;
            heap_ptr = entry_start;
            heap_end = entry_end;
        }
    }

    return max_len > 0;
}

void* alloc(size_t size) {
    uintptr_t old_ptr = heap_ptr;
    heap_ptr += size;
    if (heap_ptr > heap_end)
        return NULL;
    heap_ptr = round_up(heap_ptr);
    return (void*)old_ptr;
}
