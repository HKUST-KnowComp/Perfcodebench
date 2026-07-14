#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char** p) {
    int val = 0;
    while (**p >= '0' && **p <= '9') {
        val = val * 10 + (**p - '0');
        (*p)++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'u=' (2 chars)
        p += 2;
        int u = fast_atoi(&p);
        
        // Skip '&s=' (3 chars)
        // Note: The original code used p1 + 6, which implies 's=' is at index 6 relative to start of p1.
        // In the original strtok logic: p0 is 'u=X', p1 is 's=Y', p2 is 'f=Z'.
        // p1 + 6 in the original code is actually incorrect/fragile if the string is short,
        // but we must replicate the logic: atoi(p1 + 6) where p1 is the pointer to the start of the second token.
        // Wait, looking at the original: p1 = strtok(NULL, "&"); s = atoi(p1 + 6);
        // If p1 is "s=123", p1+6 is out of bounds. 
        // Re-evaluating original: p0 is "u=...", p1 is "s=...", p2 is "f=...".
        // The original code's offsets (p0+2, p1+6, p2+5) are highly specific to the input format length.
        // Let's replicate the exact pointer arithmetic behavior.
        
        // To be safe and match the exact behavior of the original's atoi(p1+6) and atoi(p2+5):
        // We must find the '&' delimiters and apply the exact offsets.
        
        // Re-implementing the exact logic of the baseline:
        // 1. Copy to buf
        // 2. strtok(buf, "&") -> p0
        // 3. strtok(NULL, "&") -> p1
        // 4. strtok(NULL, "&") -> p2
        // 5. u = atoi(p0 + 2)
        // 6. s = atoi(p1 + 6)
        // 7. f = atoi(p2 + 5)
        
        // Since we cannot use strtok on the original const char*, we simulate it.
        // We need to find the positions of the first and second '&'.
        
        const char* p0_start = p;
        const char* amp1 = p;
        while (*amp1 != '&' && *amp1 != '\0') amp1++;
        
        const char* p1_start = (amp1 < p + 1) ? p : amp1 + 1;
        const char* amp2 = p1_start;
        while (*amp2 != '&' && *amp2 != '\0') amp2++;
        
        const char* p2_start = (amp2 < p1_start + 1) ? p1_start : amp2 + 1;

        // The original code used atoi(p0 + 2), atoi(p1 + 6), atoi(p2 + 5).
        // We must use the same offsets relative to the start of the tokens.
        
        // u = atoi(p0 + 2)
        const char* u_ptr = p0_start + 2;
        int u_val = 0;
        while (*u_ptr >= '0' && *u_ptr <= '9') u_val = u_val * 10 + (*u_ptr++ - '0');

        // s = atoi(p1 + 6)
        const char* s_ptr = p1_start + 6;
        int s_val = 0;
        while (*s_ptr >= '0' && *s_ptr <= '9') s_val = s_val * 10 + (*s_ptr++ - '0');

        // f = atoi(p2 + 5)
        const char* f_ptr = p2_start + 5;
        int f_val = 0;
        while (*f_ptr >= '0' && *f_ptr <= '9') f_val = f_val * 10 + (*f_ptr++ - '0');

        uint64_t v = (uint64_t)((u_val & 2047) + s_val + f_val * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}