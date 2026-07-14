#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_u(const char *p) {
    uint64_t v = 0;
    p += 2;                     /* skip "u=" */
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (uint64_t)(*p - '0');
        ++p;
    }
    return v & 2047;
}

static inline uint64_t parse_s(const char *p) {
    uint64_t v = 0;
    p += 6;                     /* skip "size=" */
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (uint64_t)(*p - '0');
        ++p;
    }
    return v;
}

static inline uint64_t parse_f(const char *p) {
    uint64_t v = 0;
    p += 5;                     /* skip "flag=" */
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (uint64_t)(*p - '0');
        ++p;
    }
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        uint64_t u = parse_u(p);
        while (*p != '&') ++p;  /* find first '&' */
        ++p;
        uint64_t s = parse_s(p);
        while (*p != '&') ++p;  /* find second '&' */
        ++p;
        uint64_t f = parse_f(p);
        uint64_t v = u + s + f * 19;
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}