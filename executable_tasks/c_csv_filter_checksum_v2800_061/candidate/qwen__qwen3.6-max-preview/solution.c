#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'r'
        p++;
        // Skip id
        while (*p != ',') p++;
        p++;
        // Skip a
        while (*p != ',') p++;
        p++;
        
        // Parse b
        int b = 0;
        int neg_b = (*p == '-');
        if (neg_b) p++;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (neg_b) b = -b;
        p++; // skip ','
        
        // Parse c
        int c = 0;
        int neg_c = (*p == '-');
        if (neg_c) p++;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (neg_c) c = -c;
        p++; // skip ','
        
        uint64_t v = (uint64_t)(b + c + 22);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}