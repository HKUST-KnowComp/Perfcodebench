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
        
        // Find p0: skip 'id=' (3 chars) or similar, but logic says p0+2
        // Based on original: p0 is first segment, u = atoi(p0 + 2)
        // This implies p0 starts with 'id=' or similar 2-char prefix.
        const char* p0_start = p;
        const char* p0_end = p;
        while (*p0_end != '&' && *p0_end != '\0') p0_end++;
        int u = fast_atoi(p0_start + 2);

        // Find p1: skip 'status=' (7 chars) or similar, but logic says p1+6
        // Based on original: p1 is second segment, s = atoi(p1 + 6)
        if (*p0_end == '&') {
            const char* p1_start = p0_end + 1;
            const char* p1_end = p1_start;
            while (*p1_end != '&' && *p1_end != '\0') p1_end++;
            int s = fast_atoi(p1_start + 6);

            // Find p2: skip 'flag=' (5 chars) or similar, but logic says p2+5
            if (*p1_end == '&') {
                const char* p2_start = p1_end + 1;
                int f = fast_atoi(p2_start + 5);
                
                uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
                hash ^= (v + 1ULL);
                hash *= 1099511628211ULL;
            }
        }
    }
    return hash;
}