#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint64_t parse_u(const char *p) {
    uint64_t v = 0;
    p += 2;                       /* skip "u=" */
    while (*p >= '0' && *p <= '9') {
        v = v * 10ULL + (uint64_t)(*p - '0');
        ++p;
    }
    return v;
}

static inline uint64_t parse_s(const char *p) {
    uint64_t v = 0;
    p += 6;                       /* skip "&s=" */
    while (*p >= '0' && *p <= '9') {
        v = v * 10ULL + (uint64_t)(*p - '0');
        ++p;
    }
    return v;
}

static inline uint64_t parse_f(const char *p) {
    uint64_t v = 0;
    p += 5;                       /* skip "&f=" */
    while (*p >= '0' && *p <= '9') {
        v = v * 10ULL + (uint64_t)(*p - '0');
        ++p;
    }
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *row = rows[i];
        uint64_t u = parse_u(row);
        uint64_t s = parse_s(row);
        uint64_t f = parse_f(row);
        uint64_t v = ((u & 2047ULL) + s + f * 19ULL);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}