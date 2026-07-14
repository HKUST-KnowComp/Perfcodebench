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
        const char *s = rows[i];
        /* skip 'r' */
        ++s;
        /* parse id */
        parse_int(&s);
        /* skip ',' */
        ++s;
        /* parse a */
        parse_int(&s);
        /* skip ',' */
        ++s;
        /* parse b */
        int b = parse_int(&s);
        /* skip ',' */
        ++s;
        /* parse c */
        int c = parse_int(&s);
        /* skip ',' */
        ++s;
        /* flag starts at s */
        uint64_t v = (uint64_t)(b + c + 25);
        /* check if flag == "hot" (3 chars + null or end) */
        if (s[0] == 'h' && s[1] == 'o' && s[2] == 't' && (s[3] == '\0' || s[3] == ',' || s[3] == '\n' || s[3] == '\r')) {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
