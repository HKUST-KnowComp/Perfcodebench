#include "interface.h"

#include <stdint.h>
#include <stddef.h>

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

        // Parse a
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip comma

        // Parse b
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        p++; // skip comma

        // Parse c
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        p++; // skip comma

        uint64_t v = (uint64_t)(b + c + 29);
        
        // Check if flag is "hot"
        // Since the format is fixed, we just check if it starts with 'h'
        // and is followed by 'o' and 't'.
        if (*p == 'h' && *(p + 1) == 'o' && *(p + 2) == 't') {
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}