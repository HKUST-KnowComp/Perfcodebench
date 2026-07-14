#include "interface.h"

#include <stdint.h>
#include <stddef.h>

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
        const char* p = rows[i];
        
        // Skip 'u=' (2 chars)
        p += 2;
        int u = fast_atoi(p);
        
        // Find next '&' and skip 's=' (2 chars)
        while (*p != '&' && *p != '\0') p++;
        if (*p == '&') p += 2;
        int s = fast_atoi(p);
        
        // Find next '&' and skip 'f=' (2 chars)
        while (*p != '&' && *p != '\0') p++;
        if (*p == '&') p += 2;
        int f = fast_atoi(p);

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}