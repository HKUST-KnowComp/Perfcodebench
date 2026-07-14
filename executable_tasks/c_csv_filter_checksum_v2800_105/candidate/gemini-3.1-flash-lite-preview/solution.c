#include "interface.h"
#include <stdint.h>

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
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        // Skip 'r'
        p++;
        // Skip id
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip comma
        
        // Skip a
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip comma
        
        int b = parse_int(&p);
        p++; // skip comma
        int c = parse_int(&p);
        p++; // skip comma
        
        uint64_t v = (uint64_t)(b + c + 20);
        
        // Check for 'hot' (h=104, o=111, t=116)
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}