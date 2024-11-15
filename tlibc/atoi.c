#include "std.h"

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
