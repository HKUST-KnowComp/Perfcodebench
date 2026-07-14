#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi_skip(const char* p, int skip) {
    p += skip;
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
        
        // Find p0 (u=...) - assuming format u=X&s=Y&f=Z
        // The original code used p0+2, p1+6, p2+5 based on specific key lengths
        // u= (2 chars), s= (2 chars), f= (2 chars) is not quite right in original
        // Original: p0+2 (u=), p1+6 (s=...), p2+5 (f=...)
        // Let's replicate the exact logic of the original offsets via manual scanning
        
        const char* p0 = s;
        const char* p1 = NULL;
        const char* p2 = NULL;

        // Find first '&'
        while (*p0 && *p0 != '&') p0++;
        if (*p0 == '&') {
            p1 = p0 + 1;
            while (*p1 && *p1 != '&') p1++;
            if (*p1 == '&') {
                p2 = p1 + 1;
            }
        }

        // The original code used atoi(p0 + 2), atoi(p1 + 6), atoi(p2 + 5)
        // This implies specific key lengths: u= (2), s=XXXX (6), f=XXX (5)
        // We must match the exact behavior of the baseline.
        
        int u = fast_atoi_skip(p0, 2);
        int s_val = fast_atoi_skip(p1, 6);
        int f = fast_atoi_skip(p2, 5);

        uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}