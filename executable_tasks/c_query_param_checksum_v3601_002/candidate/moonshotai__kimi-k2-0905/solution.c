#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_u64(const char **s) {
    uint64_t v = 0;
    while (**s >= '0' && **s <= '9') {
        v = v * 10 + (uint64_t)(**s - '0');
        (*s)++;
    }
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        p += 2; // skip "u="
        uint64_t u = parse_u64(&p);
        p += 6; // skip "&s="
        uint64_t s = parse_u64(&p);
        p += 5; // skip "&f="
        uint64_t f = parse_u64(&p);
        uint64_t v = ((u & 2047ULL) + s + f * 19ULL);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}