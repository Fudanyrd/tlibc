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
    
    // enter with interrupts disabled
    cli

    mov %edi, %esi
    lea .descriptors, %edi
    add $16, %edi // skip the null descriptor
    call desc_set_base
    sub $8, %edi // skip the code segment descriptor
    add $0x200, %esi
    call desc_set_base
    lgdt .table

    // set ss, ds, es, fs, gs
    movw $GDT_ENTRY(2), %si
    movw %si, %ss
    movw %si, %ds
    movw %si, %es
    movw %si, %fs
    movw %si, %gs

    // jump to setup code: jmp.far $(base_ptr + 0x200)
    ljmp $GDT_ENTRY(1), $0x0

.code32
halt:
    // should not reach here
    hlt
.L0:
    jmp .L0

// void desc_set_base(struct SegmentDescriptor *desc, uint32_t base) {
//     desc->base_low = base & 0xFFFF;
//     desc->base_mid = (base >> 16) & 0xFF;
//     desc->base_high = (base >> 24) & 0xFF;
// }
desc_set_base:
    mov %esi, %eax
    and $0xFFFF, %eax
    movw %ax, 4(%edi) // base_low
    mov %esi, %eax
    shr $16, %eax
    and $0xFF, %eax
    movb %al, 6(%edi) // base_mid
    mov %esi, %eax
    shr $24, %eax
    movb %al, 7(%edi) // base_high
    ret

.align 16
.descriptors:
    // null descriptor
    .long 0x0
    .long 0x0 
    // code segment descriptor
    .word 0xffff // seg_limit_low
    .word 0x0000 // base_low
    .byte 0x00 // base_mid
    .byte 0x90 // P, DPL, S, Type
    .byte 0xCF // G, D/B, L, AVL, seg_limit_high
    .byte 0x00 // base_high
    // data segment descriptor
    .word 0xffff // seg_limit_low
    .word 0x0000 // base_low
    .byte 0x00 // base_mid
    .byte 0x92 // P, DPL, S, Type
    .byte 0xCF // G, D/B, L, AVL, seg_limit_high
    .byte 0x00 // base_high
.end_descriptors:

.align 16
.table:
    .word (.end_descriptors - .descriptors - 1) // size of the table in bytes
    .long .descriptors // address of the table
