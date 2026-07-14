#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'r' and the first integer (id)
        if (*p == 'r') p++;
        while (*p >= '0' && *p <= '9') p++;
        
        // Skip comma
        p++;
        
        // Parse 'a' (we don't actually use 'a', but must skip it)
        while (*p >= '0' && *p <= '9') p++;
        p++;
        
        // Parse 'b'
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        p++;
        
        // Parse 'c'
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        p++;
        
        // Check if flag is "hot"
        // Since the format is fixed, we just check if it starts with 'h'
        // and is followed by 'ot'.
        uint64_t v = (uint64_t)(b + c + 25);
        if (*p == 'h' && *(p + 1) == 'o' && *(p + 2) == 't') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}