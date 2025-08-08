.globl read_disk
.globl _start
.globl __elf_main

.code32
_start:
    // load the rest of the loader(total 2 sectors)
    // 532 is the second sector containing this bootloader.
    // FIXME: it is hard-coded.
    push $532
    push $0x1400
    call read_disk
    add $8, %esp
    // jump to the loader
    call __elf_main
