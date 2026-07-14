#include "interface.h"
#include <stdint.h>

static inline int fast_atoi(const char **sp) {
    const char *s = *sp;
    int val = 0;
    int sign = 1;
    if (*s == '-') {
        sign = -1;
        ++s;
    }
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        ++s;
    }
    *sp = s;
    return sign * val;
}

uint64_t run(const char *const *rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        p += 2;                /* skip "u=" */
        int u = fast_atoi(&p); /* p now at next '&' */
        p += 7;                /* skip "&score=" */
        int s = fast_atoi(&p); /* p now at next '&' */
        p += 6;                /* skip "&flag=" */
        int f = fast_atoi(&p); /* parse remaining */
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
