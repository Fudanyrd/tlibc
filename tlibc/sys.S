#include <syscall.h>
// use gcc -E sys.S to expand syscall numbers.
// -E enables macro expansion.
.globl main

// NOTE:
// syscall arguments:
// first:   rdi
// second:  rsi
// third:   rdx
// fourth:  r10 (WARNING: rcx for function call!)
// fifth:   r8
// sixth:   r9
// 
// syscall id is placed in %rax.

.globl sys_exit
sys_exit:
    movq $SYS_exit, %rax
    syscall

.globl sys_write
sys_write:
    movq $SYS_write, %rax
    syscall
    ret

.globl sys_pause
sys_pause:
    movq $SYS_pause, %rax
    syscall
    ret

.globl sys_read
sys_read:
    movq $SYS_read, %rax
    syscall
    ret

.globl sys_open
sys_open:
    movq $SYS_open, %rax 
    syscall
    ret

.globl sys_close
    movq $SYS_close, %rax
    syscall
    ret

.globl _start
_start:
    // the linker will put the program entry here,
    // where argc and argv are prepared before jumping to
    // main.
    // 
    // the main routine can be written in C, eventually
    // provide entire unix experience!

    // argc -> rdi
    movq (%rsp), %rdi
    // argv -> rsi
    movq %rsp, %rsi
	add $0x8, %rsi
    // jump to main
    call main
    // call exit(return_code_of_main)
    movq %rax, %rdi
    call sys_exit
    ret

.globl sys_mmap
sys_mmap:
    mov $SYS_mmap, %rax
    // pitfall: fourth arg of syscall is in r10.
    mov %rcx, %r10
    syscall 
    ret

.globl sys_munmap
sys_munmap:
    mov $SYS_munmap, %rax 
    syscall 
    ret

.globl sys_close
sys_close:
    mov $SYS_close, %rax
    syscall
    ret

.globl sys_lseek
sys_lseek:
    mov $SYS_lseek, %rax
    syscall
    ret

.globl sys_brk
sys_brk:
    mov $SYS_brk, %rax
    syscall
    ret

.globl sys_getdents
sys_getdents:
    movq $SYS_getdents, %rax
    syscall
    ret

.globl sys_execve
sys_execve:
    movq $SYS_execve, %rax
    syscall
    ret

.globl sys_fork
sys_fork:
    movq $SYS_fork, %rax
    syscall
    ret

.globl sys_dup2
sys_dup2:
    movq $SYS_dup2, %rax
    syscall
    ret

.globl sys_dup
sys_dup:
    movq $SYS_dup, %rax
    syscall
    ret

.globl sys_pipe
sys_pipe:
    movq $SYS_pipe, %rax
    syscall 
    ret

.globl sys_link
sys_link:
    movq $SYS_link, %rax
    syscall
    ret

.globl sys_fstat
sys_fstat:
    movq $SYS_fstat, %rax
    syscall
    ret

.globl sys_mkdir
sys_mkdir:
    movq $SYS_mkdir, %rax
    syscall
    ret
