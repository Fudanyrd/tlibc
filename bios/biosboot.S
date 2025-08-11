
.globl _start
.globl __elf_main

.code32
_start:
    pushl $0x1400
    pushl $532
    call bios_read_disk
    popl %eax
    popl %eax

    pushl $0x1600
    pushl $533
    call bios_read_disk

    call __elf_main
    hlt
.L0:
    jmp .L0


