#ifndef _WAIT_H_
#define _WAIT_H_

#include <stdint.h>
#define P_PID 1
#define P_ALL 0

#define WEXITED 4
#define WSTOPPED 2
#define WCONTINUED 8

typedef struct {
    int si_signo;
    int si_code; 
    int si_pid;
    int si_uid;
    void *si_addr;
    int si_status;
    /* FIXME */
    uint8_t unused[100];
} siginfo_t;

extern int sys_waitid(uint32_t idtype, uint32_t id, siginfo_t *infop, int options);

#endif // _WAIT_H_