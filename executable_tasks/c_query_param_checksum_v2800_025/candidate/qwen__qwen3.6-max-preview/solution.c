#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* p) {
    int val = 0;
    unsigned char c = *p;
    while (c >= '0' && c <= '9') {
        val = val * 10 + (c - '0');
        c = *++p;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        p += 2;
        int u = fast_atoi(p);
        while (*p != '&' && *p) p++;
        
        p += 7;
        int s = fast_atoi(p);
        while (*p != '&' && *p) p++;
        
        p += 6;
        int f = fast_atoi(p);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}