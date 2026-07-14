#include "interface.h"
#include <stdint.h>

static inline int fast_atoi(const char **s) {
    int val = 0;
    while (**s >= '0' && **s <= '9') {
        val = val * 10 + (**s - '0');
        (*s)++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        // Skip 'r' and id
        while (*p != ',') p++;
        p++;
        // Parse a, b, c
        fast_atoi(&p); p++; // skip a
        int b = fast_atoi(&p); p++;
        int c = fast_atoi(&p); p++;
        
        uint64_t v = (uint64_t)(b + c + 10);
        
        // Check for 'hot' flag
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}