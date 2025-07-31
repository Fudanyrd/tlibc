SECTIONS {
    . = 0x7C00;
    .text : {
        *(.text)
        *(.text.__x86.get_pc_thunk.bx)
    }
    .eh_frame : {
        *(.eh_frame)
    }
    .got.plt : {
        *(.got.plt)
    }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    PROVIDE(__elf_loader = 0x1200);
}
