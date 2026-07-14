#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* p, const char* end) {
    int val = 0;
    while (p < end && *p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        
        // Find p0: u=...& (offset 2 is 'u=')
        // We need to find the value after 'u=' and before the next '&'
        const char* p0_val = s + 2;
        const char* p0_end = p0_val;
        while (*p0_end != '&' && *p0_end != '\0') p0_end++;
        int u = fast_atoi(p0_val, p0_end);

        // Find p1: s=...& (offset 6 is 's=')
        // The baseline used p1 + 6, implying 's=' is at index 4 relative to the start of p1
        // Since p1 starts after the first '&', we look for the next '&' then skip 's='
        const char* p1_start = p0_end;
        if (*p1_start == '&') p1_start++;
        const char* p1_val = p1_start + 2; // skip 's='
        const char* p1_end = p1_val;
        while (*p1_end != '&' && *p1_end != '\0') p1_end++;
        int s_val = fast_atoi(p1_val, p1_end);

        // Find p2: f=... (offset 5 is 'f=')
        const char* p2_start = p1_end;
        if (*p2_start == '&') p2_start++;
        const char* p2_val = p2_start + 2; // skip 'f='
        const char* p2_end = p2_val;
        while (*p2_end != '\0' && *p2_end != '&') p2_end++;
        int f = fast_atoi(p2_val, p2_end);

        uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}