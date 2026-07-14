#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* p) {
    int v = 0;
    int neg = (*p == '-');
    p += neg;
    if (*p == '+') p++;
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (*p - '0');
        p++;
    }
    return neg ? -v : v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        int u = fast_atoi(r + 2);
        
        const char* p = r;
        while (*p != '&' && *p != '\0') p++;
        int s = fast_atoi(p + 7);
        
        p++;
        while (*p != '&' && *p != '\0') p++;
        int f = fast_atoi(p + 6);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
