// a routine for jumping to setup code.
// jump_to_setup(void *base_ptr, void *heap_end)
// see also: https://kernel.org/doc/html/latest/arch/x86/boot.html#running-the-kernel

// jump to an absolute address outside this "segment" 
// see also: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
// volume 2A, jump instruction
#define jmp_far FIXME

#define GDT_ENTRY(n)  \
	((n) << 3)

// extern void jump_to_setup(void *base_ptr, void *heap_end);
.globl jump_to_setup
.globl halt

.code32
jump_to_setup:
    pop %eax       // return address
    pop %edi       // base_ptr
    pop %edx       // heap_end
    mov %edx, %esp // use heap_end as the stack pointer
    push %eax
    
    // (Vol.3, 11.9.2) Switching Back to Real-Address Mode
    // enter with interrupts disabled
    cli

    // Transfer program control to a readable segment that has a limit of 64 KBytes
    //// use far jump to switch to the code segment
    lgdt .table
    ljmp $GDT_ENTRY(1), $next

.code32
next:
    // load segment registers with the same proper selector
    movw $GDT_ENTRY(2), %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %fs

    // clear PE bit
    mov %cr0, %eax
    and $~(1 << 0), %eax
    mov %eax, %cr0

    // jump to setup code: jmp.far $(base_ptr + 0x200)
    ljmp $0x220, $0x12000

.code32
halt:
    // should not reach here
    hlt
.L0:
    jmp .L0

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
    .byte 0x40 // G, D/B, L, AVL, seg_limit_high
    .byte 0x00 // base_high
    // data segment descriptor
    // suitable for real-address mode
    .word 0xffff // seg_limit_low
    .word 0x0000 // base_low
    .byte 0x00 // base_mid
    .byte 0x92 // P, DPL, S, Type
    .byte 0x40 // G, D/B, L, AVL, seg_limit_high
    .byte 0x00 // base_high
.end_descriptors:

.align 16
.table:
    .word (.end_descriptors - .descriptors - 1) // size of the table in bytes
    .long .descriptors // address of the table
