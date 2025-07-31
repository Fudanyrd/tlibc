# read_disk(void *, uint32_t)
.globl read_disk
# void __elf_main(void)
.globl __elf_main

.code32
.globl _start
_start:
    push $532 
    push $0x1400
    call read_disk
    add $8, %esp
    push $539
    call __elf_main
