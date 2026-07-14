#include "interface.h"
#include <stdint.h>
#include <string.h>

static inline int parse_int(const char **p) {
    const char *s = *p;
    int neg = 0;
    int val = 0;
    if (*s == '-') { neg = 1; ++s; }
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        ++s;
    }
    *p = s;
    return neg ? -val : val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        /* skip 'r' */
        ++p;
        /* skip id */
        while (*p != ',') ++p;
        ++p;
        /* skip a */
        while (*p != ',') ++p;
        ++p;
        /* parse b */
        int b = parse_int(&p);
        ++p; /* skip comma */
        /* parse c */
        int c = parse_int(&p);
        ++p; /* skip comma */
        /* flag starts at p */
        uint64_t v = (uint64_t)(b + c + 11);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ',' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
