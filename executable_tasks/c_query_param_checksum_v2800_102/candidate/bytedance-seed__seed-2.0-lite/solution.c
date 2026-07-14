#include "interface.h"
#include <stdint.h>
#include <string.h>

static inline int parse_decimal(const char *restrict p) {
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p++ - '0');
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *row = rows[i];
        int u = parse_decimal(row + 2);
        const char *and1 = strchr(row, '&');
        int s = parse_decimal(and1 + 7);
        const char *and2 = strchr(and1 + 1, '&');
        int f = parse_decimal(and2 + 6);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}