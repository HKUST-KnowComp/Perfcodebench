#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Optimized manual parser to replace sscanf
// Format: r%d,%d,%d,%d,%7s
// We need: b, c, and flag (to check if 'hot')

static inline uint32_t fast_atoi(const char** s) {
    uint32_t val = 0;
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
        
        // Skip a
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;
        
        // Parse b
        uint32_t b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;
        
        // Parse c
        uint32_t c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;
        
        // Skip d
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;
        
        // Check flag (hot)
        // The flag starts at p. We check if it matches 'hot'
        uint64_t v = (uint64_t)(b + c + 10);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}