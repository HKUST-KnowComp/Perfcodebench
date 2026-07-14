#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_row(const char *s) {
    /* parse r<id> */
    uint64_t id = 0;
    s += 1;                     /* skip 'r' */
    while (*s >= '0' && *s <= '9') { id = id * 10 + (*s++ - '0'); }
    s++;                        /* skip ',' */

    /* parse a */
    int a = 0, sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') { a = a * 10 + (*s++ - '0'); }
    a *= sign;
    s++;                        /* skip ',' */

    /* parse b */
    int b = 0; sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') { b = b * 10 + (*s++ - '0'); }
    b *= sign;
    s++;                        /* skip ',' */

    /* parse c */
    int c = 0; sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') { c = c * 10 + (*s++ - '0'); }
    c *= sign;
    s++;                        /* skip ',' */

    /* parse flag: hot or not */
    uint64_t v = (uint64_t)(b + c + 17);
    if (*s == 'h' && s[1] == 'o' && s[2] == 't') {
        v += 29ULL;
    }
    return v + 1ULL;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        uint64_t v = parse_row(rows[i]);
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}
