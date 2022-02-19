ENTRY(_start)

PHDRS {
    multiboot PT_LOAD;
    text PT_LOAD;
    data PT_LOAD;
    bss PT_LOAD;
}

SECTIONS {
    . = 1M;

    .multiboot ALIGN(4K) : AT(ADDR(.multiboot)) {
        KEEP(*(.multiboot))
    } :multiboot

    .text ALIGN(4K) : AT(ADDR(.text)) {
        *(.text)
    } :text

    .rodata ALIGN(4K) : AT(ADDR(.rodata)){
        *(.rodata)
    } :data

    .data ALIGN(4K) : AT(ADDR(.data)) {
        *(.data)
    } :data

    .bss ALIGN(4K) : AT(ADDR(.bss)) {
        *(COMMON)
        *(.bss)
        *(.stack)
    } :bss

    /DISCARD/ : {
        *(.note.gnu.build-id)
    }

    end = .;
}
