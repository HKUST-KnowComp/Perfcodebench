#include "interface.h"
#include <stdint.h>

static inline int parse_int_until(const char* p, char end, const char** out_next) {
    int val = 0;
    while (*p != end) {
        val = val * 10 + (*p - '0');
        p++;
    }
    *out_next = p + 1;
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        int u = parse_int_until(p + 2, '&', &p);
        int s = parse_int_until(p + 6, '&', &p);
        int f = parse_int_until(p + 5, '\0', &p);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}