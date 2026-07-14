#include "interface.h"

#include <stdint.h>
#include <string.h>

static inline int parse_int(const char **p) {
    const char *s = *p;
    int neg = 0;
    if (*s == '-') { neg = 1; ++s; }
    int v = 0;
    while (*s >= '0' && *s <= '9') {
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
        /* parse id */
        parse_int(&p);
        /* skip ',' */
        ++p;
        /* parse a */
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
        /* flag starts at p */
        uint64_t v = (uint64_t)(b + c + 20);
        /* check if flag == "hot" (3 chars + null or end) */
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ',' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
