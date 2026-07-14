#include "interface.h"

#include <stdint.h>

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
    const uint64_t multiplier = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'r'
        p++; 
        
        // Skip id
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip comma
        
        // Skip a
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip comma
        
        // Parse b
        int b = fast_atoi(&p);
        p++; // skip comma
        
        // Parse c
        int c = fast_atoi(&p);
        p++; // skip comma
        
        uint64_t v = (uint64_t)(b + c + 16);
        
        // Check if flag is "hot"
        // Since it's a CSV, we check if it starts with 'h' and is followed by 'ot' and then end of string or comma
        if (*p == 'h' && *(p + 1) == 'o' && *(p + 2) == 't' && (*(p + 3) == '\0' || *(p + 3) == ',')) {
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}