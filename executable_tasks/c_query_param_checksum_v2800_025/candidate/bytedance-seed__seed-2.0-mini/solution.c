#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int parse_digits(const char* str) {
    int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiply = 1099511628211ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        
        // Locate first parameter separator
        char* amp1 = strchr(s, '&');
        if (!amp1) continue;
        int u = parse_digits(s + 2);
        
        // Locate second parameter separator
        char* second_start = amp1 + 1;
        char* amp2 = strchr(second_start, '&');
        if (!amp2) continue;
        int s_val = parse_digits(second_start + 6);
        
        // Parse third parameter value
        char* third_start = amp2 + 1;
        int f_val = parse_digits(third_start + 5);
        
        // Compute combined value and update hash
        uint64_t v = (uint64_t)((u & 2047) + s_val + f_val * 19);
        hash ^= (v + 1ULL);
        hash *= multiply;
    }
    
    return hash;
}