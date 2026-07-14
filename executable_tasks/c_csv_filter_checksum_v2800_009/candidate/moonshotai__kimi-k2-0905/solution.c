#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char *)rows[i];

        /* skip "r" and parse id (unused) */
        ++s;
        while (*s >= '0' && *s <= '9') ++s;
        ++s; /* skip comma */

        /* parse a (unused) */
        while (*s >= '0' && *s <= '9') ++s;
        ++s; /* skip comma */

        /* parse b */
        int b = 0, neg = 0;
        if (*s == '-') { neg = 1; ++s; }
        while (*s >= '0' && *s <= '9') b = b * 10 + (*s++ - '0');
        if (neg) b = -b;
        ++s; /* skip comma */

        /* parse c */
        int c = 0; neg = 0;
        if (*s == '-') { neg = 1; ++s; }
        while (*s >= '0' && *s <= '9') c = c * 10 + (*s++ - '0');
        if (neg) c = -c;
        ++s; /* skip comma */

        /* check flag: only need first byte to distinguish "hot" */
        uint64_t v = (uint64_t)(b + c + 16);
        if (*s == 'h' && s[1] == 'o' && s[2] == 't') v += 29;

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}