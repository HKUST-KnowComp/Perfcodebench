#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Fast integer parsing for positive integers
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
        
        // The original logic expects three specific parameters:
        // p0: u (offset 2 from start of p0)
        // p1: s (offset 6 from start of p1)
        // p2: f (offset 5 from start of p2)
        // Format assumed: "u=...&s=...&f=..."
        
        // Find first parameter (u)
        const char* p0 = s;
        const char* eq0 = NULL;
        const char* amp0 = NULL;
        
        // Find '=' for first param
        while (*p0 && *p0 != '=') p0++;
        if (*p0 == '=') {
            const char* val_start = p0 + 1;
            // Find '&' to end first param
            const char* next = val_start;
            while (*next && *next != '&') next++;
            amp0 = next;
            
            // The original code used atoi(p0 + 2). 
            // In the baseline, p0 was the start of the token (e.g., "u=123").
            // p0 + 2 points to the start of the digits.
            int u = fast_atoi(p0 + 2);

            // Find second parameter (s)
            if (amp0 && *amp0 == '&') {
                const char* p1 = amp0 + 1;
                const char* eq1 = NULL;
                const char* amp1 = NULL;
                
                while (*p1 && *p1 != '=') p1++;
                if (*p1 == '=') {
                    // Original: atoi(p1 + 6). 
                    // If p1 is "s=12345", p1+6 is the start of digits.
                    int s_val = fast_atoi(p1 + 6);

                    // Find third parameter (f)
                    const char* next2 = p1;
                    while (*next2 && *next2 != '&') next2++;
                    if (next2 && *next2 == '&') {
                        const char* p2 = next2 + 1;
                        // Original: atoi(p2 + 5). 
                        // If p2 is "f=123", p2+5 is the start of digits.
                        int f_val = fast_atoi(p2 + 5);

                        uint64_t v = (uint64_t)((u & 2047) + s_val + f_val * 19);
                        hash ^= (v + 1ULL);
                        hash *= 1099511628211ULL;
                    }
                }
            }
        }
    }
    return hash;
}