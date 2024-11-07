/** wrap something in double quotes */
#define TO_STR(reg) #reg

#define REGISTER64(_)                              \
		/* return value */                         \
        _(rax)                                     \
		/* callee saved */                         \
        _(rbx)                                     \
		/* fourtch parameter*/                     \
        _(rcx)                                     \
		/* third parameter*/                       \
        _(rdx)                                     \
		/* second parameter*/                      \
        _(rsi)                                     \
		/* first parameter*/                       \
        _(rdi)                                     \
		/* callee saved */                         \
        _(rbp)                                     \
		/* stack pointer */                        \
        _(rsp)                                     \
		/* fifth parameter */                      \
        _(r8)                                      \
		/* sixth parameter */                      \
        _(r9)                                      \
		/* caller saved */                         \
        _(r10)                                     \
        _(r11)                                     \
		/* callee saved */                         \
        _(r12)                                     \
        _(r13)                                     \
        _(r14)                                     \
        _(r15)                                     \

#define REGISTER32(_)                              \
		/* return value */                         \
        _(eax)                                     \
		/* callee saved */                         \
        _(ebx)                                     \
		/* fourtch parameter*/                     \
        _(ecx)                                     \
		/* third parameter*/                       \
        _(edx)                                     \
		/* second parameter*/                      \
        _(esi)                                     \
		/* first parameter*/                       \
        _(edi)                                     \
		/* callee saved */                         \
        _(ebp)                                     \
		/* stack pointer */                        \
        _(esp)                                     \
		/* fifth parameter */                      \
        _(r8d)                                     \
		/* sixth parameter */                      \
        _(r9d)                                     \
		/* caller saved */                         \
        _(r10d)                                    \
        _(r11d)                                    \
		/* callee saved */                         \
        _(r12d)                                    \
        _(r13d)                                    \
        _(r14d)                                    \
        _(r15d)                                    \

#define REGISTER16(_)                              \
		/* return value */                         \
        _(ax)                                      \
		/* callee saved */                         \
        _(bx)                                      \
		/* fourtch parameter*/                     \
        _(cx)                                      \
		/* third parameter*/                       \
        _(dx)                                      \
		/* second parameter*/                      \
        _(si)                                      \
		/* first parameter*/                       \
        _(di)                                      \
		/* callee saved */                         \
        _(bp)                                      \
		/* stack pointer */                        \
        _(sp)                                      \
		/* fifth parameter */                      \
        _(r8w)                                     \
		/* sixth parameter */                      \
        _(r9w)                                     \
		/* caller saved */                         \
        _(r10w)                                    \
        _(r11w)                                    \
		/* callee saved */                         \
        _(r12w)                                    \
        _(r13w)                                    \
        _(r14w)                                    \
        _(r15w)                                    \

#define REGISTER8(_)                               \
		/* return value */                         \
        _(al)                                      \
		/* callee saved */                         \
        _(bl)                                      \
		/* fourtch parameter*/                     \
        _(cl)                                      \
		/* third parameter*/                       \
        _(dl)                                      \
		/* second parameter*/                      \
        _(sil)                                     \
		/* first parameter*/                       \
        _(dil)                                     \
		/* callee saved */                         \
        _(bpl)                                     \
		/* stack pointer */                        \
        _(spl)                                     \
		/* fifth parameter */                      \
        _(r8b)                                     \
		/* sixth parameter */                      \
        _(r9b)                                     \
		/* caller saved */                         \
        _(r10b)                                    \
        _(r11b)                                    \
		/* callee saved */                         \
        _(r12b)                                    \
        _(r13b)                                    \
        _(r14b)                                    \
        _(r15b)                                    \
