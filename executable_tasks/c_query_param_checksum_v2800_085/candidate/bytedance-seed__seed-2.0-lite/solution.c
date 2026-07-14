#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int parse_int(const char* p) {
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p++ - '0');
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        const char* amp1 = strchr(row, '&');
        const char* amp2 = strchr(amp1 + 1, '&');
        int u = parse_int(row + 2);
        int s = parse_int(amp1 + 1 + 6);
        int f = parse_int(amp2 + 1 + 5);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}