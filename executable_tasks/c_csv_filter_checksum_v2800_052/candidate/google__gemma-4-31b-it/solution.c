#include "interface.h"

#include <stdint.h>
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
        
        // Skip 'r' and the first integer (id)
        if (*p == 'r') p++;
        while (*p && *p != ',') p++;
        if (*p == ',') p++;
        
        // Skip second integer (a)
        while (*p && *p != ',') p++;
        if (*p == ',') p++;
        
        // Parse third integer (b)
        int b = fast_atoi(&p);
        while (*p && *p != ',') p++;
        if (*p == ',') p++;
        
        // Parse fourth integer (c)
        int c = fast_atoi(&p);
        while (*p && *p != ',') p++;
        if (*p == ',') p++;
        
        // Check for "hot" flag
        uint64_t v = (uint64_t)(b + c + 13);
        if (*p == 'h' && *(p + 1) == 'o' && *(p + 2) == 't') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}