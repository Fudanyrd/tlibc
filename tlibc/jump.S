.globl setjmp
setjmp:
    movq %rax, 0x8(%rdi)
    movq %rbx, 0x10(%rdi)
    movq %rcx, 0x18(%rdi)
    movq %rdx, 0x20(%rdi)
    movq %rsi, 0x28(%rdi)
    movq %rdi, 0x30(%rdi)
    movq %rbp, 0x38(%rdi)
    movq %rsp, 0x40(%rdi)
    movq %r8, 0x48(%rdi)
    movq %r9, 0x50(%rdi)
    movq %r10, 0x58(%rdi)
    movq %r11, 0x60(%rdi)
    movq %r12, 0x68(%rdi)
    movq %r13, 0x70(%rdi)
    movq %r14, 0x78(%rdi)
    movq %r15, 0x80(%rdi)
    movq 0x0(%rsp), %rax
    movq %rax, 0x0(%rdi)
    movq $0, %rax
    ret

.globl longjmp
longjmp:
    // recover registers
    movq 0x10(%rdi), %rbx
    movq 0x18(%rdi), %rcx
    movq 0x20(%rdi), %rdx
    movq 0x38(%rdi), %rbp
    movq 0x48(%rdi), %r8
    movq 0x50(%rdi), %r9
    movq 0x58(%rdi), %r10
    movq 0x60(%rdi), %r11
    movq 0x68(%rdi), %r12
    movq 0x70(%rdi), %r13
    movq 0x78(%rdi), %r14
    movq 0x80(%rdi), %r14
    // set return addr of longjmp
    movq 0x0(%rdi), %rax
    movq %rax, 0x0(%rsp)
    movq %rsi, %rax
    ret
