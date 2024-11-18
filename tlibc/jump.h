#ifndef _JUMP_H_
#define _JUMP_H_

#ifndef __ASSEMBER__

#include <stdint.h>

/** A light-weight snapshot of a program */
struct jumpbuf {
    uintptr_t rip;  /*  0 */
    uint64_t rax;  /*   8 */
    uint64_t rbx;  /*  16 */
    uint64_t rcx;  /*  24 */
    uint64_t rdx;  /*  32 */
    uint64_t rsi;  /*  40 */
    uint64_t rdi;  /*  48 */
    uint64_t rbp;  /*  56 */
    uintptr_t rsp; /*  64 */
    uint64_t r8 ;  /*  72 */
    uint64_t r9 ;  /*  80 */
    uint64_t r10;  /*  88 */
    uint64_t r11;  /*  96 */
    uint64_t r12;  /* 104 */
    uint64_t r13;  /* 112 */
    uint64_t r14;  /* 120 */
    uint64_t r15;  /* 128 */
};
typedef struct jumpbuf jmp_buf;

int setjmp(jmp_buf *jb);
void longjmp(jmp_buf *jb, int val);

#endif // __ASSEMBER__
#endif // _JUMP_H_
