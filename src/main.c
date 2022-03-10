#include "alloc.h"
#include "common.h"
#include "multiboot.h"
#include "renderer.h"
#include <stdbool.h>
#include <stdint.h>

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64
#define PS2_DISABLE_PORT2 0xa7

static uint8_t in8(uint16_t port) {
    uint8_t rv;
    __asm__ volatile("inb %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

static void out8(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(data));
}

static void handle_keyboard(renderer* renderer) {
    int32_t timeout = 0;
    while (timeout >= 0) {
        if (!(in8(PS2_STATUS) & 1)) {
            --timeout;
            continue;
        }
        timeout = 1024;

        switch (in8(PS2_DATA)) {
        case 0x13: // R
            renderer_generate_random_scene(renderer);
            renderer_clear_screen(renderer);
            break;
        case 0x1f: // S
            renderer_toggle_draw_scanline(renderer);
            break;
        }
    }
}

void start(const multiboot_info_t* mbi) {
    out8(PS2_COMMAND, PS2_DISABLE_PORT2); // disable mouse

    if (!alloc_init(mbi))
        return;

    random_seed(42);

    renderer* r = renderer_create(mbi);
    if (!r)
        return;

    while (true) {
        handle_keyboard(r);
        renderer_render(r);
    }
}
