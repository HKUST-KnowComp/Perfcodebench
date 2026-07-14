#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int parse_digits(const char *p, const char **end_out) {
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    *end_out = p;
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *curr = rows[i];
        const char *end;
        
        int u = parse_digits(curr + 2, &end);
        int s = parse_digits(end + 1 + 6, &end);
        int f = parse_digits(end + 1 + 5, &end);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}