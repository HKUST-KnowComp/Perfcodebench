#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int parse_int(const char **pp) {
    const char *p = *pp;
    int n = 0;
    while (*p >= '0' && *p <= '9') {
        n = n * 10 + (*p - '0');
        ++p;
    }
    *pp = p;
    return n;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    size_t i = 0;
    size_t end4 = row_count & ~(size_t)3;

    for (; i < end4; i += 4) {
        const char *r0 = rows[i];
        const char *r1 = rows[i + 1];
        const char *r2 = rows[i + 2];
        const char *r3 = rows[i + 3];

        /* Row 0 */
        const char *p = r0 + 2;
        int u0 = parse_int(&p);
        p += 7;
        int s0 = parse_int(&p);
        p += 6;
        int f0 = parse_int(&p);
        uint64_t v0 = (uint64_t)((u0 & 2047) + s0 + f0 * 19);
        hash ^= (v0 + 1ULL);
        hash *= prime;

        /* Row 1 */
        p = r1 + 2;
        int u1 = parse_int(&p);
        p += 7;
        int s1 = parse_int(&p);
        p += 6;
        int f1 = parse_int(&p);
        uint64_t v1 = (uint64_t)((u1 & 2047) + s1 + f1 * 19);
        hash ^= (v1 + 1ULL);
        hash *= prime;

        /* Row 2 */
        p = r2 + 2;
        int u2 = parse_int(&p);
        p += 7;
        int s2 = parse_int(&p);
        p += 6;
        int f2 = parse_int(&p);
        uint64_t v2 = (uint64_t)((u2 & 2047) + s2 + f2 * 19);
        hash ^= (v2 + 1ULL);
        hash *= prime;

        /* Row 3 */
        p = r3 + 2;
        int u3 = parse_int(&p);
        p += 7;
        int s3 = parse_int(&p);
        p += 6;
        int f3 = parse_int(&p);
        uint64_t v3 = (uint64_t)((u3 & 2047) + s3 + f3 * 19);
        hash ^= (v3 + 1ULL);
        hash *= prime;
    }

    for (; i < row_count; ++i) {
        const char *r = rows[i];
        const char *p = r + 2;
        int u = parse_int(&p);
        p += 7;
        int s = parse_int(&p);
        p += 6;
        int f = parse_int(&p);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= prime;
    }

    return hash;
}
