#include "interface.h"
#include <stdint.h>
#include <string.h>

/*
 * Fast, branch-free CSV parser for rows of the form:
 *     r<id>,<a>,<b>,<c>,<flag>\0
 * We need b, c and whether flag=="hot".
 */
static inline uint64_t parse_row(const char *s)
{
    const unsigned char *p = (const unsigned char *)s;
    uint32_t b = 0, c = 0;
    uint32_t comma = 0;

    /* skip "r" and parse id (we don’t need id) */
    ++p;
    while (*p >= '0' && *p <= '9') ++p;
    ++p; /* skip comma */

    /* parse a (we don’t need a) */
    while (*p >= '0' && *p <= '9') ++p;
    ++p; /* skip comma */

    /* parse b */
    while (*p >= '0' && *p <= '9') b = b * 10 + (*p++ - '0');
    ++p; /* skip comma */

    /* parse c */
    while (*p >= '0' && *p <= '9') c = c * 10 + (*p++ - '0');
    ++p; /* skip comma */

    /* detect flag=="hot" without strcmp */
    uint32_t hot = 0;
    hot |= (p[0] == 'h') ? 1 : 0;
    hot |= (p[1] == 'o') ? 2 : 0;
    hot |= (p[2] == 't') ? 4 : 0;
    hot = (hot == 7) ? 29 : 0;

    uint64_t v = (uint64_t)(b + c + 29) + hot;
    return v;
}

uint64_t run(const char* const* rows, size_t row_count)
{
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    size_t i = 0;
    /* manual 4-way unroll to hide latency and increase ILP */
    for (; i + 3 < row_count; i += 4) {
        uint64_t v0 = parse_row(rows[i+0]) + 1;
        uint64_t v1 = parse_row(rows[i+1]) + 1;
        uint64_t v2 = parse_row(rows[i+2]) + 1;
        uint64_t v3 = parse_row(rows[i+3]) + 1;

        hash ^= v0; hash *= prime;
        hash ^= v1; hash *= prime;
        hash ^= v2; hash *= prime;
        hash ^= v3; hash *= prime;
    }
    /* tail */
    for (; i < row_count; ++i) {
        uint64_t v = parse_row(rows[i]) + 1;
        hash ^= v;
        hash *= prime;
    }
    return hash;
}