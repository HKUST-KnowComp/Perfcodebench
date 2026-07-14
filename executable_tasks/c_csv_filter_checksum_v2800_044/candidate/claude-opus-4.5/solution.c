#include "interface.h"
#include <stdint.h>

static inline int parse_int(const char **p) {
    const char *s = *p;
    int neg = 0;
    if (*s == '-') { neg = 1; ++s; }
    int v = 0;
    while ((unsigned)(*s - '0') < 10u) {
        v = v * 10 + (*s - '0');
        ++s;
    }
    *p = s;
    return neg ? -v : v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        /* skip 'r' */
        ++p;
        /* parse id (unused) */
        parse_int(&p);
        /* skip ',' */
        ++p;
        /* parse a (unused) */
        parse_int(&p);
        /* skip ',' */
        ++p;
        /* parse b */
        int b = parse_int(&p);
        /* skip ',' */
        ++p;
        /* parse c */
        int c = parse_int(&p);
        /* skip ',' */
        ++p;
        /* flag comparison: check if starts with "hot" and next char is '\0' or non-alpha */
        uint64_t v = (uint64_t)(b + c + 28);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ',' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
