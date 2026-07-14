#include "interface.h"

#include <stdint.h>

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
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;
        
        // Parse c
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;
        
        // Check if flag is "hot"
        // We only need to check if it starts with 'h', 'o', 't' and ends or is followed by something else
        uint64_t v = (uint64_t)(b + c + 25);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == '\r' || p[3] == '\n')) {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}