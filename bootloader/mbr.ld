SECTIONS {
    . = 0x7F00;
    .text : {
        *(.text)
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
