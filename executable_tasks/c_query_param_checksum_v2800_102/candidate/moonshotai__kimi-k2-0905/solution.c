#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_u(const char *p) {
    uint64_t v = 0;
    for (int i = 0; i < 4; ++i) {
        char c = p[i];
        if (c < '0' || c > '9') break;
        v = v * 10 + (c - '0');
    }
    return v;
}

static inline uint64_t parse_s_or_f(const char *p) {
    uint64_t v = 0;
    for (int i = 0; i < 10; ++i) {
        char c = p[i];
        if (c < '0' || c > '9') break;
        v = v * 10 + (c - '0');
    }
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t m = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *row = rows[i];
        uint64_t u = parse_u(row + 2);
        uint64_t s = parse_s_or_f(row + 9);
        uint64_t f = parse_s_or_f(row + 18);
        uint64_t v = ((u & 2047ULL) + s + f * 19ULL) + 1ULL;
        hash ^= v;
        hash *= m;
    }
    return hash;
}