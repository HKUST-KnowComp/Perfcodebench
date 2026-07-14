#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int parse_int(const char *restrict p) {
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        // Parse u value, skip "u=" prefix
        p += 2;
        int u = parse_int(p);
        // Locate first & separator
        while (*p != '&') p++;
        // Parse s value, skip 6-character s token prefix
        p++;
        p += 5;
        int s = parse_int(p);
        // Locate second & separator
        while (*p != '&') p++;
        // Parse f value, skip 5-character f token prefix
        p++;
        p += 4;
        int f = parse_int(p);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}