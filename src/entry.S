#define ASM_FILE
#include "multiboot.h"

#define FLAGS (MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE)
#define CHECKSUM -(MULTIBOOT_HEADER_MAGIC + FLAGS)
#define STACK_SIZE 0x4000

    .section .multiboot, "a"
    .align MULTIBOOT_HEADER_ALIGN
    .long MULTIBOOT_HEADER_MAGIC
    .long FLAGS
    .long CHECKSUM

    // MULTIBOOT_AOUT_KLUDGE
    .long 0 // header_addr
    .long 0 // load_addr
    .long 0 // load_end_addr
    .long 0 // bss_end_addr
    .long 0 // entry_addr

    // MULTIBOOT_VIDEO_MODE
    .long 0  // mode_type
    .long 0  // width
    .long 0  // height
    .long 32 // depth

    .text
    .globl _start
_start:
    movl $stack_top, %esp
    pushl %ebx
    call start

    cli
loop:
    hlt
    jmp loop

    .section .stack, "aw", @nobits
    .skip STACK_SIZE
stack_top:
