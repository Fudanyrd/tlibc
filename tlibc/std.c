#include <stdint.h>
#include <stdarg.h>
#include "std.h"

static const char *digits = "0123456789abcdef";
static unsigned int putint(char *dst, int val);
static unsigned int putuint(char *dst, unsigned int val);
static unsigned int puthex(char *dst, unsigned int val);
static unsigned int putptr(char *dst, unsigned long val);
static unsigned int putl(char *dst, long val);
static unsigned int putstr(char *dst, const char *src);

void Puts(const char *s)
{
    sys_write(1, s, Strlen(s));
}

/**
 * Supported format: d(int), u(unsigned), x(for int32, unsigned32), 
 * p(for pointer, ulong), l(long), s(string).
 */
unsigned int Sprintf(char *dst, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    unsigned ret = 0;

    for (int i = 0; fmt[i] != 0;) {
        if (fmt[i] == '%') {
            switch (fmt[i + 1]) {
            case 'd': {
                ret += putint(dst + ret, va_arg(arg, int));
                break;
            }
            case 'u': {
                ret += putuint(dst + ret, va_arg(arg, unsigned int));
                break;
            }
            case 'x': {
                ret += puthex(dst + ret, va_arg(arg, unsigned int));
                break;
            }
            case 'p': {
                ret += putptr(dst + ret, va_arg(arg, unsigned long));
                break;
            }
            case 'l': {
                ret += putl(dst + ret, va_arg(arg, long));
                break;
            }
            case 's': {
                ret += putstr(dst + ret, va_arg(arg, char *));
                break;
            }
            default: {
                dst[ret++] = '%';
                i--;
            }
            }
            i += 2;
        } else {
            dst[ret] = fmt[i];
            ret++;
            i++;
        }
    }
    dst[ret] = 0;

    va_end(arg);
    return ret;
}

static unsigned int putint(char *dst, int val)
{
    unsigned int ret = 0;
    char buf[16];
    unsigned int v;
    if (val < 0) {
        v = -val;
        dst[ret++] = '-';
    } else {
        // put negative sign
        v = val;
    }

    int i = 0;
    while (v > 0) {
        buf[i++] = digits[v % 0xa];
        v /= 0xa;
    }

    while (i > 0) {
        i--;
        dst[ret++] = buf[i];
    }
    return ret;
}

static unsigned int putuint(char *dst, unsigned int val)
{
    unsigned int ret = 0;
    char buf[16];
    int i = 0;
    while (val > 0) {
        buf[i++] = digits[val % 0xa];
        val /= 0xa;
    }

    while (i > 0) {
        i--;
        dst[ret++] = buf[i];
    }
    return ret;
}

static unsigned int puthex(char *dst, unsigned int val)
{
    unsigned int ret = 0;
    char buf[8];
    int i = 0;

    dst[ret++] = '0';
    dst[ret++] = 'x';
    while (val > 0) {
        buf[i++] = digits[val & 0xf];
        val = val >> 4;
    }

    while (i > 0) {
        i--;
        dst[ret++] = buf[i];
    }
    return ret;
}

static unsigned int putptr(char *dst, unsigned long val)
{
    unsigned int ret = 0;
    char buf[16];
    int i = 0;

    dst[ret++] = '0';
    dst[ret++] = 'x';
    while (val > 0) {
        buf[i++] = digits[val & 0xf];
        val = val >> 4ul;
    }

    while (i > 0) {
        i--;
        dst[ret++] = buf[i];
    }
    return ret;
}

static unsigned int putl(char *dst, long val)
{
    unsigned int ret = 0;
    unsigned long v;
    char buf[24];
    if (val < 0l) {
        dst[ret++] = '-';
        v = -val;
    } else {
        v = val;
    }

    int i = 0;
    while (v > 0) {
        buf[i++] = digits[v % 0xa];
        v /= 0xa;
    }
    while (i > 0) {
        i--;
        dst[ret++] = buf[i];
    }

    return ret;
}

static unsigned int putstr(char *dst, const char *src)
{
    unsigned int ret = 0;
    if (src == (const char *)0) {
        dst[ret++] = '(';
        dst[ret++] = 'n';
        dst[ret++] = 'u';
        dst[ret++] = 'l';
        dst[ret++] = 'l';
        dst[ret++] = ')';
        return ret;
    }

    while (*src != 0) {
        dst[ret++] = *src;
        src++;
    }
    return ret;
}

unsigned int Printf(const char *fmt, ...)
{
    static char buf[2048];
    char *dst = (char *)buf;
    va_list arg;
    va_start(arg, fmt);
    unsigned ret = 0;

    for (int i = 0; fmt[i] != 0;) {
        if (fmt[i] == '%') {
            switch (fmt[i + 1]) {
            case 'd': {
                ret += putint(dst + ret, va_arg(arg, int));
                break;
            }
            case 'u': {
                ret += putuint(dst + ret, va_arg(arg, unsigned int));
                break;
            }
            case 'x': {
                ret += puthex(dst + ret, va_arg(arg, unsigned int));
                break;
            }
            case 'p': {
                ret += putptr(dst + ret, va_arg(arg, unsigned long));
                break;
            }
            case 'l': {
                ret += putl(dst + ret, va_arg(arg, int64_t));
                break;
            }
            case 's': {
                ret += putstr(dst + ret, va_arg(arg, char *));
                break;
            }
            default: {
                dst[ret++] = '%';
                i--;
            }
            }
            i += 2;
        } else {
            dst[ret] = fmt[i];
            ret++;
            i++;
        }
    }
    dst[ret] = 0;
    va_end(arg);

    sys_write(1, dst, ret);
    return ret;
}

size_t Strlen(const char *s) {
    size_t i = 0;
    for (; s[i] != 0; i++) {
    }
    return i;
}

int atoi(const char *nptr) {
    int ret = 0;
    for (; *nptr; nptr++) {
        if (*nptr >= '0' && *nptr <= '9') {
            ret *= 10; 
            ret += *nptr - '0';
        }
    }

    return ret;
}

void *Memset(void *addr, int val, size_t len) {
    for (size_t i = 0; i < len; i++) {
        *(uint8_t *)(addr + i) = val & 0xff;
    }
}

/** Execute a single job */
static void exec_single(job_t *job) __attribute__((noreturn));

/** Execute a job recursively(do not return) */
static void exec_recur_noret(job_t *job, int cur, int total, int stdout_fd) __attribute__((noreturn));

static void exec_single(job_t *job) {
    Assert(job != NULL);
    if (job->stdin_fo) {
        int fd = sys_open(job[0].stdin_fo, O_RDONLY);
        if (fd > 0) {
            sys_dup2(fd, 0);
        }
    }

    if (job->stdout_fo) {
        int fd = sys_open(job[0].stdout_fo, O_CREAT | O_RDWR);
        if (fd > 0) {
            sys_dup2(fd, 1);
        }
    }

    if (job->stderr_fo) {
        int fd = sys_open(job[0].stderr_fo, O_CREAT | O_RDWR);
        if (fd > 0) {
            sys_dup2(fd, 2);
        }
    }

    sys_execve((char *)job->exe, (char **)job->argv, NULL);
}

int exec_job(job_t *job, int cnt) {
    Assert(cnt >= 0);
    Assert(cnt == 0 || job != NULL);

    int pid = sys_fork();

    if (pid < 0) {
        return -1;
    }

    if (pid == 0) {
        exec_recur_noret(job, cnt - 1, cnt, 1);
        sys_exit(1);
    }

    return 0;
}

static void exec_recur_noret(job_t *job, int cur, int total, int stdout_fd) {
    Assert(cur <= total);

    if (cur == 0) {
        exec_single(job);
        sys_exit(1);
    }

    int stat;
    int pip[2];
    if (sys_pipe((int *)pip) != 0) {
        sys_exit(1);
    }

    int pid = sys_fork();
    if (pid < 0) {
        sys_exit(1);
    }

    if (pid == 0) {
        // child proc
        sys_dup2(pip[1], 1);
        sys_close(pip[0]);
        sys_close(pip[1]);
        exec_recur_noret(job, cur - 1, total, 1);
    } else {
        // parent proc
        sys_dup2(pip[0], 0);
        sys_close(pip[0]);
        sys_close(pip[1]);
        exec_single(&job[cur]);
    }

    sys_exit(1);
}

char *Strcpy(char *dst, const char *src) {
    if (!dst) {
        return NULL;
    }

    for (; *src; dst++, src++) {
        *dst = *src;
    }
}
