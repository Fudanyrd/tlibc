SECTIONS {
    . = 0x1200;
    .text : {
        *(.text)
        *(.text.*)
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
}
