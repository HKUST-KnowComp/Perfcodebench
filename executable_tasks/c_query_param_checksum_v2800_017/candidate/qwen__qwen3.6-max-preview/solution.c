#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* s) {
    int v = 0;
    int sign = 1;
    if (*s == '-') { sign = -1; s++; }
    while (*s >= '0' && *s <= '9') {
        v = v * 10 + (*s - '0');
        s++;
    }
    return v * sign;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        int u = fast_atoi(s + 2);
        
        const char* p1 = s;
        while (*p1 != '&') p1++;
        int sv = fast_atoi(p1 + 7);
        
        const char* p2 = p1 + 1;
        while (*p2 != '&') p2++;
        int f = fast_atoi(p2 + 6);
        
        uint64_t v = (uint64_t)((u & 2047) + sv + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}