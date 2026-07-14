#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int parse_int(const char **p) {
    int val = 0;
    while (**p >= '0' && **p <= '9') {
        val = val * 10 + (**p - '0');
        (*p)++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        // Parse u parameter, skip "u=" prefix
        p += 2;
        int u = parse_int(&p);
        // Skip delimiter and "speed=" prefix to parse s
        p++;
        p += 6;
        int s = parse_int(&p);
        // Skip delimiter and "fuel=" prefix to parse f
        p++;
        p += 5;
        int f = parse_int(&p);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= FNV_PRIME;
    }
    return hash;
}