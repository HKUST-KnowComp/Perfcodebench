#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char **p) {
    int val = 0;
    const char *s = *p;
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    *p = s;
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        
        // Skip "u="
        p += 2;
        int u = fast_atoi(&p);
        
        // Skip "&score="
        // p is currently at '&', skip 7 chars: "&score="
        p += 7;
        int s = fast_atoi(&p);
        
        // Skip "&flag="
        // p is currently at '&', skip 6 chars: "&flag="
        p += 6;
        int f = fast_atoi(&p);

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}