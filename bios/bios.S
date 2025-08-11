/* routine for reading bios disk. */

/* int bios_read_disk(uint32_t sector); */
.globl bios_read_disk

#define GDT_ENTRY(n)  \
	((n) << 3)

.code32
bios_read_disk:
    /*
     * step 0: save clobbered registers.
     * and store the sector to read.
     */

    /* eax := sector */
    movl 4(%esp), %eax 
    push %ebx
    push %ecx
    push %edx
    push %esi
    push %edi
    push %ebp
    movl $ksector, %ebx
    /* ksector := sector */
    movl %eax, (%ebx) 

    /*
     * step 1: jump to 16-bit.
     * also see: bzsetup.S 
     */
    cli
    lgdt .table
    ljmp $GDT_ENTRY(1), $step2

.code16
step2:
    /*
     * step 2: clear PE
     * and set data segments.
     */
    movl $GDT_ENTRY(2), %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    mov %cr0, %eax
    and $~(1 << 0), %eax
    mov %eax, %cr0

    /*
     * step 3: jump to do read_disk
     */

    ljmp $0x0, $do_read_disk

.code16
do_read_disk:
    /*
     * step 4: read the sector into a fixed
     * address (0x1000 in my configuration)
     * 
     * we are now in real-mode, set data-segment
     * to zero. the accessible memory is 0-1MiB.
     */
    xorl %eax, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    
    movw $disk_address_packet, %si
    xorw %ax, %ax
    movw %ax, 4(%si)
    inc %ax
    /* mode := non-zero */
    movb %al, -1(%si)
    /* # blocks := 1 */
    movw %ax, 2(%si) 
    /* size, reserved */
    movw $0x0010, (%si) 
    /* absolute address of sector */
    movl ksector, %ebx
    movl %ebx, 8(%si)
    movl ksector_high, %ebx
    movl %ebx, 12(%si)
    /* the segment of buffer address */
    movw $0x100, 6(%si)
    /* bios intr function */
    movb $0x42, %ah
    /* 
     * trigger intr, 
     * input: 
     *   ds:si := address of packet,
     *   dl := drive number
     *   ah := 0x42
     * output: ax
     */
    movb $0x80, %dl
    int $0x13
    /* ax = 0 means successful. */
    shr $8, %ax

    /*
     * set PE and jump to 32-bit code.
     */

    mov %cr0, %ebx
    orl $1, %ebx
    mov %ebx, %cr0

    ljmp $GDT_ENTRY(3), $restore

.code32
restore:
    /*
     * use the 32-bit data segment
     * Now in protected-mode again.
     * 
     * use bx to do so because ax now holds return
     * value - result of bios interrupt.
     */
    cli
    movl $GDT_ENTRY(4), %ebx
    movw %bx, %ds
    movw %bx, %es
    movw %bx, %fs
    movw %bx, %gs
    movw %bx, %ss

    /*
     * restore context, return.
     */
    pop %ebp
    pop %edi
    pop %esi
    pop %edx
    pop %ecx
    pop %ebx
    ret

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
    .long 0x0
ksector_high:
    .long 0x0

.align 16
.descriptors:
    // null descriptor
    .long 0x0
    .long 0x0 
    // code segment descriptor
    // suitable for real-address mode
    // - Limit = 64 KBytes (0FFFFH)
    // - Byte granular (G = 0)
    // - Expand up (E = 0)
    // - Writable (W = 1)
    // - Present (P = 1)
    // - Base = any value (0x0)
    .word 0xffff // seg_limit_low
    .word 0x0000 // base_low
    .byte 0x00 // base_mid
    .byte 0x9a // P, DPL, S, Type
    .byte 0x00 // G, D/B, L, AVL, seg_limit_high
    .byte 0x00 // base_high
    // data segment descriptor
    // suitable for real-address mode
    .word 0xffff // seg_limit_low
    .word 0x0000 // base_low
    .byte 0x00 // base_mid
    .byte 0x92 // P, DPL, S, Type
    .byte 0x00 // G, D/B, L, AVL, seg_limit_high
    .byte 0x00 // base_high
    
    // segment descriptor suitable for protected-mode.
    //// code 
    .word 0xffff // seg_limit_low
    .word 0x0000 // base_low
    .byte 0x00 // base_mid
    .byte 0x9a // P, DPL, S, Type
    .byte 0xcf // G, D/B, L, AVL, seg_limit_high
    .byte 0x00 // base_high
    //// data
    .word 0xffff // seg_limit_low
    .word 0x0000 // base_low
    .byte 0x00 // base_mid
    .byte 0x92 // P, DPL, S, Type
    .byte 0xcf // G, D/B, L, AVL, seg_limit_high
    .byte 0x00 // base_high

.end_descriptors:

.align 16
.table:
    .word (.end_descriptors - .descriptors - 1) // size of the table in bytes
    .long .descriptors // address of the table

