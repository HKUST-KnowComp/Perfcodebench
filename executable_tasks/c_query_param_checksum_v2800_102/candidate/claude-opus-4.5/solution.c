#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_int(const char **pp) {
    const char *p = *pp;
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    *pp = p;
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        /* skip "u=" */
        p += 2;
        int u = parse_int(&p);
        /* skip "&score=" */
        p += 7;
        int s = parse_int(&p);
        /* skip "&flag=" */
        p += 6;
        int f = parse_int(&p);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
