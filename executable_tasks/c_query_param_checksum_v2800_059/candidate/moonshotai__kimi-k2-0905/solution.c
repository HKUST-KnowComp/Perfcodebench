#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_int(const char *p, const char *end) {
    uint64_t v = 0;
    while (p < end && *p >= '0' && *p <= '9') {
        v = v * 10 + (uint64_t)(*p - '0');
        ++p;
    }
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t mul = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];

        /* u=...&size=...&flag=... */
        /* skip "u=" */
        s += 2;
        const char *u_end = s;
        while (*u_end != '&') ++u_end;
        uint64_t u = parse_int(s, u_end);

        /* skip "&size=" */
        s = u_end + 6;
        const char *s_end = s;
        while (*s_end != '&') ++s_end;
        uint64_t sz = parse_int(s, s_end);

        /* skip "&flag=" */
        s = s_end + 6;
        const char *f_end = s;
        while (*f_end) ++f_end;
        uint64_t f = parse_int(s, f_end);

        uint64_t v = ((u & 2047ULL) + sz + f * 19ULL) + 1ULL;
        hash ^= v;
        hash *= mul;
    }
    return hash;
}