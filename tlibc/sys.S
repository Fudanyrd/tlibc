#include <syscall.h>
// use gcc -E sys.S to expand syscall numbers.
// -E enables macro expansion.
.globl main

#ifdef __X86_64__

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

.globl sys_openat
sys_openat:
    movq $SYS_openat, %rax 
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
    // envp -> rdx
    movq %rdi, %rax
    addq $2, %rax
    shlq $3, %rax
    movq %rsp, %rdx
    addq %rax, %rdx
    // jump to main
    xor %rax, %rax
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

.globl sys_vfork
sys_vfork:
    movq $SYS_vfork, %rax
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

.globl sys_waitid
sys_waitid:
    movq $SYS_waitid, %rax
    movq %rcx, %r10
    syscall
    ret

.globl sys_chdir
sys_chdir:
    movq $SYS_chdir, %rax
    syscall
    ret

.globl sys_getcwd
sys_getcwd:
    movq $SYS_getcwd, %rax
    syscall
    ret

.globl sys_kill
sys_kill:
    movq $SYS_kill, %rax
    syscall
    ret

.globl sys_getpid
sys_getpid:
    movq $SYS_getpid, %rax
    syscall
    ret

.globl sys_nanosleep
sys_nanosleep:
    movq $SYS_nanosleep, %rax
    syscall
    ret

#endif // __X86_64__

#ifdef __AARCH64__

.globl sys_exit
sys_exit:
    mov w8, #SYS_exit
    svc #0

.globl sys_openat
sys_openat:
    mov w8, #sys_openat
    svc #0
    ret

.globl sys_write
sys_write:
    mov w8, #SYS_write
    svc #0
    ret

.globl sys_read
sys_read:
    mov w8, #SYS_read
    svc #0
    ret

.globl _start
_start:
    // the linker will put the program entry here,
    // where argc and argv are prepared before jumping to
    // main.
    // 
    // the main routine can be written in C, eventually
    // provide entire unix experience!

    // argc -> x0, x0 = *(int *)sp
	ldr x0, [sp]
    // argv -> x1, x1 = sp + 0x8
	mov x1, sp
    add x1, x1, #0x8
    // exit(main(argc, argv));
    bl main
    bl sys_exit
    ret

.globl sys_mmap
sys_mmap:
    mov w8, #SYS_mmap
    svc #0 
    ret

.globl sys_vfork
sys_vfork:
    mov w8, #SYS_vfork
    svc #0
    ret

.globl sys_munmap
sys_munmap:
    mov w8, #SYS_munmap 
    svc #0 
    ret

.globl sys_close
sys_close:
    mov w8, #SYS_close
    svc #0
    ret

.globl sys_lseek
sys_lseek:
    mov w8, #SYS_lseek
    svc #0
    ret

.globl sys_brk
sys_brk:
    mov w8, #SYS_brk
    svc #0
    ret

.globl sys_execve
sys_execve:
    mov w8, #SYS_execve
    svc #0
    ret

.globl sys_dup
sys_dup:
    mov w8, #SYS_dup
    svc #0
    ret

.globl sys_fstat
sys_fstat:
    mov w8, #SYS_fstat
    svc #0
    ret

.globl sys_waitid
sys_waitid:
    mov w8, #SYS_waitid
    svc #0
    ret

.globl sys_chdir
sys_chdir:
    mov w8, #SYS_chdir
    svc #0
    ret

.globl sys_getcwd
sys_getcwd:
    mov w8, #SYS_getcwd
    svc #0
    ret

.globl sys_kill
sys_kill:
    mov w8, #SYS_kill
    svc #0
    ret

.globl sys_getpid
sys_getpid:
    mov w8, #SYS_getpid
    svc #0
    ret

.globl sys_nanosleep
sys_nanosleep:
    mov w8, #SYS_nanosleep
    svc #0
    ret

#endif // __AARCH64__
