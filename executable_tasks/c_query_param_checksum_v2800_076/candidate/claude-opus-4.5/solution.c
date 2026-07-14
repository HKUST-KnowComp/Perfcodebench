#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_int(const char **pp) {
    const char *p = *pp;
    int neg = 0;
    if (*p == '-') { neg = 1; ++p; }
    int v = 0;
    while ((unsigned)(*p - '0') < 10u) {
        v = v * 10 + (*p - '0');
        ++p;
    }
    *pp = p;
    return neg ? -v : v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *r = rows[i];
        /* skip "u=" */
        r += 2;
        int u = 0;
        int neg = 0;
        if (*r == '-') { neg = 1; ++r; }
        while ((unsigned)(*r - '0') < 10u) {
            u = u * 10 + (*r - '0');
            ++r;
        }
        if (neg) u = -u;
        /* skip '&' and "score=" (1 + 6 = 7) */
        r += 7;
        int s = 0;
        neg = 0;
        if (*r == '-') { neg = 1; ++r; }
        while ((unsigned)(*r - '0') < 10u) {
            s = s * 10 + (*r - '0');
            ++r;
        }
        if (neg) s = -s;
        /* skip '&' and "flag=" (1 + 5 = 6) */
        r += 6;
        int f = 0;
        neg = 0;
        if (*r == '-') { neg = 1; ++r; }
        while ((unsigned)(*r - '0') < 10u) {
            f = f * 10 + (*r - '0');
            ++r;
        }
        if (neg) f = -f;
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
