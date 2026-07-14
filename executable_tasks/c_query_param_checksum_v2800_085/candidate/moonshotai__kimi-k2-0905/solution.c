#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_u(const char *p) {
    uint64_t x = 0;
    for (p += 2; *p != '&'; ++p)
        x = x * 10 + (uint64_t)(*p - '0');
    return x & 2047ULL;
}

static inline uint64_t parse_s(const char *p) {
    uint64_t x = 0;
    for (p += 6; *p != '&'; ++p)
        x = x * 10 + (uint64_t)(*p - '0');
    return x;
}

static inline uint64_t parse_f(const char *p) {
    uint64_t x = 0;
    for (p += 5; *p; ++p)
        x = x * 10 + (uint64_t)(*p - '0');
    return x;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const char *const *end = rows + row_count;

    for (; rows != end; ++rows) {
        const char *r = *rows;
        uint64_t u = parse_u(r);

        const char *p = r + 2;
        while (*p != '&') ++p;
        ++p;
        uint64_t s = parse_s(p);

        while (*p != '&') ++p;
        ++p;
        uint64_t f = parse_f(p);

        uint64_t v = u + s + f * 19;
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}