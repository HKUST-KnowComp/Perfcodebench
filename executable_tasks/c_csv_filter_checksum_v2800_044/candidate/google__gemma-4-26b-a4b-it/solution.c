#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'r' and parse id
        if (*p == 'r') p++;
        while (*p >= '0' && *p <= '9') p++;
        
        // Skip ',' and parse a
        if (*p == ',') p++;
        while (*p >= '0' && *p <= '9') p++;
        
        // Skip ',' and parse b
        if (*p == ',') p++;
        uint64_t b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        
        // Skip ',' and parse c
        if (*p == ',') p++;
        uint64_t c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        
        // Skip ',' and check flag
        if (*p == ',') p++;
        
        uint64_t v = b + c + 28;
        
        // Check if flag is "hot"
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == ',' || p[3] == '\0' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}