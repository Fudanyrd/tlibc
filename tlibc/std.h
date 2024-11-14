#ifndef __STDIO_H_
#define __STDIO_H_ 1

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/** Can be used in freestanding env */
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

/** Assembly Code **/
#include "sys.h"

/** stdio.h **/

struct buffered_reader {
    char buf[2048];
    int start;
    int end;
};
extern bool fdgets(struct buffered_reader *br, char *dst, int fd);

extern void Puts(const char *fmt);
extern unsigned int Sprintf(char *dst, const char *fmt, ...);
extern unsigned int Printf(const char *fmt, ...);

/** string.h **/

extern size_t Strlen(const char *fmt);
extern char *Strcpy(char *dst, const char *src);
extern void *Memset(void *addr, int val, size_t len);

/** stdlib.h */

extern int atoi(const char *nptr);
typedef struct job {
    char *stdin_fo;   // redirent stdin to
    char *stdout_fo;  // redirent stdout to
    char *stderr_fo;  // redirent stderr to
    char *exe;     // executable
    char *argv[64];   // argv
    int used;         // # bytes in the buffer
    bool pipe;        // pipe to next prog?
    char buf[3545];  // buffer to store some data
} job_t;

extern void Execve(char *exe, char **argv);
extern int exec_job(job_t *job, int cnt);
extern int system(const char *cmd);

/** fcntl.h */
#include "fcntl.h"

/** mman.h */
#include "mman.h"

/** assert.h */
#define Assert(cond) \
    do {  \
        if(!(cond)) { \
            Printf("Assertion failure at %s:%d\n", __FILE__, __LINE__); \
            sys_exit(1); \
        }   \
    } while(0)

#define Static_assert(cond) \
    do { \
        switch (0) { \
        case (0): \
        case (cond): \
        } \
    } while(0)

#endif // __STDIO_H_
