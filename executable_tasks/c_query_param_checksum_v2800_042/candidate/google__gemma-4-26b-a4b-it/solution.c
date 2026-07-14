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
        const char* s = rows[i];
        
        // The structure is assumed to be: u=X&s=Y&f=Z
        // We need to find the values after 'u=', 's=', and 'f='
        // Based on the original logic:
        // p0 + 2 (u=), p1 + 6 (s=), p2 + 5 (f=)
        // This implies fixed offsets or specific key lengths.
        
        // Optimized manual scan to avoid strtok/strcpy overhead
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

        // The original code used specific offsets relative to the start of tokens:
        // p0 + 2, p1 + 6, p2 + 5
        // This suggests keys like 'u=' (2), 's=XXXX=' (6?), 'f=XXX=' (5?)
        // However, the original code used atoi(p0+2), etc.
        // We must replicate the exact logic: find the token, skip N chars, parse.
        
        // Re-implementing the exact logic of the baseline but without the buffer copy
        // We need to find the start of each token manually because strtok modifies the string.
        // Since we can't modify the input, we find the delimiters.
        
        // Re-calculating token starts manually to mimic strtok behavior
        const char* t0 = s;
        const char* t1 = NULL;
        const char* t2 = NULL;

        const char* amp1 = NULL;
        for(const char* c = s; *c; c++) if(*c == '&') { amp1 = c; break; }
        
        if (amp1) {
            t1 = amp1 + 1;
            const char* amp2 = NULL;
            for(const char* c = t1; *c; c++) if(*c == '&') { amp2 = c; break; }
            if (amp2) {
                t2 = amp2 + 1;
            }
        }

        // If tokens weren't found, the original code would crash or behave UB.
        // We assume valid input per the task constraints.
        int u = fast_atoi(t0 + 2);
        int s_val = fast_atoi(t1 + 6);
        int f = fast_atoi(t2 + 5);

        uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}