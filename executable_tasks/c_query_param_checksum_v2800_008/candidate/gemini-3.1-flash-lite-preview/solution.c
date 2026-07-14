#include "interface.h"
#include <stdint.h>

static inline int fast_atoi(const char* p) {
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        
        // Format: u=...&s=...&f=...
        // Skip 'u=' (2 chars)
        int u = fast_atoi(s + 2);
        
        // Find '&s='
        while (*s != '&') s++;
        s += 3; // Skip '&s='
        int val_s = fast_atoi(s);
        
        // Find '&f='
        while (*s != '&') s++;
        s += 3; // Skip '&f='
        int val_f = fast_atoi(s);
        
        uint64_t v = (uint64_t)((u & 2047) + val_s + val_f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}