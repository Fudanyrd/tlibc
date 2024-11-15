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
static unsigned int putul(char *dst, unsigned long val);

void Puts(const char *s)
{
    sys_write(1, s, Strlen(s));
}

// if buffer is empty, fill it
static bool fill_buf(struct buffered_reader *br, int fd) {
    if (br->start == br->end) {
        br->start = br->end = 0;
        // fill the queue
        char *cp = br->buf;
        long ret = sys_read(fd, cp, sizeof(br->buf));

        // EOF
        if (ret <= 0) {
            return false;
        }
        br->end += ret;
        br->end = br->end % sizeof(br->buf);
    }

    return true;
}

// returns current dst position
static char *copy_buf(struct buffered_reader *br, char *dst) {
    for (; br->start != br->end; ) {
        // do not copy \0
        bool succ = false;
        if (br->buf[br->start]) {
            *dst = br->buf[br->start];
            succ = (*dst == '\n');
            dst++;
        }
        br->start += 1;
        br->start = br->start % sizeof(br->buf);

        if (succ) {
            *dst = 0;
            return dst;
        }
    }

    return dst;
}

bool fdgets(struct buffered_reader *br, char *dst, int fd) {
    // the queue is empty
    while (1) {
        if (!fill_buf(br, fd)) {
            return false;
        }
        dst = copy_buf(br, dst);
        if (*dst == 0) {
            return true;
        }
    }
}

/**
 * Supported format: d(int), u(unsigned), x(for int32, unsigned32), 
 * p(for pointer, ulong), l(long), s(string), L(unsigned long).
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
            case 'L': {
                ret += putul(dst + ret, va_arg(arg, unsigned long));
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

    if (v == 0) {
        *dst = '0';
        return 1;
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
    if (val == 0) {
        *dst = '0';
        return 1;
    }

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
    if (val == 0) {
        buf[ret++] = '0';
        return ret;
    }
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

static unsigned int putul(char *dst, unsigned long val) {
    if (val == 0) {
        *dst = '0';
        return 1;
    }

    // longest ulong is 18446_74407_37095_51615
    static char buf[24];
    unsigned int ret = 0;
    while (val != 0) {
        buf[ret++] = digits[val % 10];
        val /= 10;
    }

    // reverse-copy back to dst
    for (unsigned int i = 0; i < ret; i++) {
        dst[i] = buf[ret - 1 - i];
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
            case 'L': {
                ret += putul(dst + ret, va_arg(arg, unsigned long));
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
