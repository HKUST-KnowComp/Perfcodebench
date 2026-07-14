#include "interface.h"

#include <stdint.h>- 
#include <stdio.h>
#include <string.h>

static inline int fast_atoi(const char** s) {
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
        const char* p = rows[i];
        
        // Skip 'r'
        if (*p == 'r') p++;
        
        // Skip id
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;
        
        // Parse a
        fast_atoi(&p);
        if (*p == ',') p++;
        
        // Parse b
        int b = fast_atoi(&p);
        if (*p == ',') p++;
        
        // Parse c
        int c = fast_atoi(&p);
        if (*p == ',') p++;
        
        // Check for "hot"
        uint64_t v = (uint64_t)(b + c + 11);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}