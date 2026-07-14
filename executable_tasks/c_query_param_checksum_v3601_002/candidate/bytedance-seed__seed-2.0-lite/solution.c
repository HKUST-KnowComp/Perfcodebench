#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int parse_int_until(const char* restrict p, const char end) {
    int val = 0;
    while (*p != end && *p != '\0') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_MULT = 1099511628211ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Parse first parameter (u=*) matching original p0+2 offset
        int u = parse_int_until(p + 2, '&');
        
        // Advance to start of second parameter segment
        while (*p != '&') p++;
        p++;
        
        // Parse second parameter (*=*) matching original p1+6 offset
        int s = parse_int_until(p + 6, '&');
        
        // Advance to start of third parameter segment
        while (*p != '&') p++;
        p++;
        
        // Parse third parameter (*=*) matching original p2+5 offset
        int f = parse_int_until(p + 5, '\0');
        
        // Exact original hash computation preserved for correctness
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= FNV_MULT;
    }
    return hash;
}