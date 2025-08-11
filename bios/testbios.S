
.globl _start

.code32
_start:
    pushl $539
    call bios_read_disk
    hlt
.L0:
    jmp .L0


