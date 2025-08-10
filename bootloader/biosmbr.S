// To learn about how to drive bios disk,
// download the source code of grub-2.12,
// and read grub-core/boot/i386/pc/boot.S
.globl __elf_start
.globl _start
.globl start32

#define CR0_PE          0x00000001

#define GDT_ENTRY(n)  \
	((n) << 3)

#define SEG_NULLASM   \
	.word 0, 0;       \
	.byte 0, 0, 0, 0

#define SEG_ASM(type, base, lim)                          \
	.word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
	.byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
	(0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

.code16
_start:
  cli

  // initialize bios disk
  movb $0x41, %ah
  movw $0x55aa, %bx
  movw $0x80, %dx
  movb $0xff, %al
  movw $disk_address_packet, %si
  
  int $0x13 
  jc .L1 // if fail, goto .L1
  cmpw $0xaa55, %bx
  jne .L1
  andw $1, %cx
  jz .L1

  xorw %ax, %ax
  movw %ax, 4(%si)
  inc %ax
  movb %al, -1(%si) // mode
  movw %ax, 2(%si) // blocks
  movw $0x0010, (%si) // size, reserved
  // sector to read
  movl ksector, %ebx
  movl %ebx, 8(%si)
  movl ksector_high, %ebx
  movl %ebx, 12(%si)
  // the segment of buffer address
  movw $0x120, 6(%si)
  
  movb $0x42, %ah
  int $0x13

  xorw    %ax, %ax
  movw    %ax, %ds
  movw    %ax, %es
  movw    %ax, %ss

  lgdt    gdtdesc
  movl    %cr0, %eax
  orl     $CR0_PE, %eax
  movl    %eax, %cr0
  ljmp    $GDT_ENTRY(1), $start32

.L1: 
  // fail
  hlt
.L0:
  jmp .L0

.code32
start32:
  movw    $GDT_ENTRY(2), %ax
  movw    %ax, %ds
  movw    %ax, %es
  movw    %ax, %ss

  movl $0xa000, %esp
  nop
  movl $0x1200, %eax
  nop
  jmp *%eax

# GDT
.p2align 2
gdt:
  SEG_NULLASM
  SEG_ASM(0xA, 0x0, 0xffffffff)
  SEG_ASM(0x2, 0x0, 0xffffffff)

gdtdesc:
  .word   (gdtdesc - gdt - 1)
  .long   gdt

mode:
    .byte 0
disk_address_packet:
sectors:
	.long	0
heads:
	.long	0
cylinders:
	.word	0
sector_start:
	.byte	0
head_start:
	.byte	0
cylinder_start:
	.word	0
ksector:
    .long 531 // read the same sector as mbr.c
ksector_high:
    .long 0x0
