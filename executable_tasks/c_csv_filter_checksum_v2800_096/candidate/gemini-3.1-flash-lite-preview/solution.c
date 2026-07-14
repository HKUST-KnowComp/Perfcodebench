#include "interface.h"
#include <stdint.h>

static inline int parse_int(const char **ptr) {
    int val = 0;
    while (**ptr >= '0' && **ptr <= '9') {
        val = val * 10 + (**ptr - '0');
        (*ptr)++;
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
        
        // Parse a, b, c
        parse_int(&p); p++; // skip comma
        int b = parse_int(&p); p++; // skip comma
        int c = parse_int(&p); p++; // skip comma
        
        // Check for 'hot'
        uint64_t v = (uint64_t)(b + c + 11);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}