#include "std.h"

size_t Strlen(const char *s) {
    size_t i = 0;
    for (; s[i] != 0; i++) {
    }
    return i;
}

void *Memset(void *addr, int val, size_t len) {
    for (size_t i = 0; i < len; i++) {
        *(uint8_t *)(addr + i) = val & 0xff;
    }
    return addr;
}

char *Strcpy(char *dst, const char *src) {
    if (!dst) {
        return NULL;
    }

    for (; *src; dst++, src++) {
        *dst = *src;
    }

    // set null terminator, return.
    *dst = '\0';
    return dst;
}
