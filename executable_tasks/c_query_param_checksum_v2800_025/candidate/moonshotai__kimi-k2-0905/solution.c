#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_decimal(const char *p, const char *end) {
    uint64_t v = 0;
    while (p < end && *p >= '0' && *p <= '9') {
        v = v * 10ULL + (uint64_t)(*p++ - '0');
    }
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];

        /* u=...&s=...&f=... */
        const char *p0 = s + 2;                       /* skip "u=" */
        const char *e0 = p0;
        while (*e0 != '&') ++e0;
        uint64_t u = parse_decimal(p0, e0) & 2047ULL;

        const char *p1 = e0 + 3;                      /* skip "&s=" */
        const char *e1 = p1;
        while (*e1 != '&') ++e1;
        uint64_t s_val = parse_decimal(p1, e1);

        const char *p2 = e1 + 3;                      /* skip "&f=" */
        uint64_t f = parse_decimal(p2, p2 + 16);      /* no need to find end */

        uint64_t v = u + s_val + f * 19ULL;
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}