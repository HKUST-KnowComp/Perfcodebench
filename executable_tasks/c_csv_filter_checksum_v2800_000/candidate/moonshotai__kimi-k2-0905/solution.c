#include "interface.h"
#include <stdint.h>
#include <string.h>

/*
 * Fast, branch-free CSV parser for rows of the form:
 *     r<id>,<a>,<b>,<c>,<flag>\n
 * We need b, c and whether flag=="hot".
 * Everything is ASCII; we parse in one left-to-right pass.
 */

static inline uint64_t parse_row(const char *s)
{
    const unsigned char *p = (const unsigned char *)s;

    /* skip leading 'r' */
    ++p;

    /* ----- parse id ----- */
    uint32_t id = 0;
    while (*p >= '0' && *p <= '9') {
        id = id * 10U + (*p++ - '0');
    }
    /* expect comma */
    ++p;

    /* ----- parse a (unused) ----- */
    while (*p >= '0' && *p <= '9') ++p;
    ++p;

    /* ----- parse b ----- */
    uint32_t b = 0;
    while (*p >= '0' && *p <= '9') {
        b = b * 10U + (*p++ - '0');
    }
    ++p;

    /* ----- parse c ----- */
    uint32_t c = 0;
    while (*p >= '0' && *p <= '9') {
        c = c * 10U + (*p++ - '0');
    }
    ++p;

    /* ----- parse flag (3 chars max: "hot" or other) ----- */
    uint32_t hot_flag = 0;
    /* We need exactly 'h','o','t' followed by non-alpha or end */
    if (p[0]=='h' && p[1]=='o' && p[2]=='t') {
        unsigned char n = p[3];
        if ((n<'A' || (n>'Z'&&n<'a')||n>'z') && n!='\0' && n!='\n' && n!=',') {
            hot_flag = 1;
        }
    }

    uint64_t v = (uint64_t)(b + c + 7);
    v += hot_flag * 29ULL;
    return v;
}

uint64_t run(const char* const* rows, size_t row_count)
{
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    size_t i = 0;
    /* manual 4-way unroll to increase ILP */
    for (; i + 3 < row_count; i += 4) {
        uint64_t v0 = parse_row(rows[i+0]) + 1ULL;
        uint64_t v1 = parse_row(rows[i+1]) + 1ULL;
        uint64_t v2 = parse_row(rows[i+2]) + 1ULL;
        uint64_t v3 = parse_row(rows[i+3]) + 1ULL;

        hash ^= v0; hash *= prime;
        hash ^= v1; hash *= prime;
        hash ^= v2; hash *= prime;
        hash ^= v3; hash *= prime;
    }
    /* tail */
    for (; i < row_count; ++i) {
        uint64_t v = parse_row(rows[i]) + 1ULL;
        hash ^= v;
        hash *= prime;
    }
    return hash;
}