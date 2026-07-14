#include "interface.h"

#include <stdint.h>

static inline int parse_int(const char *s, const char *limit) {
    int sign = 1;
    int val = 0;
    if (*s == '-') {
        sign = -1;
        ++s;
    }
    while (s < limit && *s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        ++s;
    }
    return val * sign;
}

uint64_t run(const char *const *rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t mul = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];

        /* u=...&s=...&f=...  */
        /* skip "u=" */
        p += 2;
        const char *u_end = p;
        while (*u_end != '&') ++u_end;
        int u = parse_int(p, u_end);
        p = u_end + 1 + 2;              /* skip "&s=" */

        const char *s_end = p;
        while (*s_end != '&') ++s_end;
        int s = parse_int(p, s_end);
        p = s_end + 1 + 2;              /* skip "&f=" */

        const char *f_end = p;
        while (*f_end != '\0') ++f_end;
        int f = parse_int(p, f_end);

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= mul;
    }
    return hash;
}