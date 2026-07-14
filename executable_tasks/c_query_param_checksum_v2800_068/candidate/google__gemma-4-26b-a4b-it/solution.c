#include "interface.h"
#include <stdint.h>
#include <stddef.h>

// Optimized implementation: Single-pass scan without strcpy or strtok
// This avoids heap/stack copies and redundant scanning of the same characters.

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
        int u = 0, s_val = 0, f = 0;
        
        // The baseline logic assumes a fixed structure: 
        // p0: 'u=...' (offset 2), p1: 's=...' (offset 6), p2: 'f=...' (offset 5)
        // However, strtok/atoi logic is sensitive to the exact position.
        // We replicate the exact logic of the baseline: 
        // p0 = first segment, p1 = second, p2 = third.
        
        const char* p0 = s;
        const char* p1 = NULL;
        const char* p2 = NULL;
        
        // Find first '&'
        while (*p0 && *p0 != '&') p0++;
        
        if (*p0 == '&') {
            p1 = p0 + 1;
            // Find second '&'
            while (*p1 && *p1 != '&') p1++;
            
            if (*p1 == '&') {
                p2 = p1 + 1;
            }
        }

        // The baseline uses atoi(p0 + 2), atoi(p1 + 6), atoi(p2 + 5)
        // This implies specific key lengths: 'u=' (2), 's=...' (6?), 'f=...' (5?)
        // Wait, the baseline offsets are hardcoded: p0+2, p1+6, p2+5.
        // We must follow these exact offsets to preserve correctness.
        
        // Note: The baseline code is actually quite fragile (it assumes p1 is at least 6 chars long).
        // We replicate the exact pointer arithmetic used in the baseline.
        
        // Re-implementing the exact logic of the baseline to ensure identical checksum:
        // We need to find the start of the segments as strtok does.
        
        // Since we cannot use strtok (it modifies the string), and we cannot use strcpy (too slow),
        // we simulate the behavior of strtok by finding the '&' delimiters.
        
        // Re-calculating based on the baseline's specific offsets:
        // p0 is the start of the string.
        // p1 is the start of the string after the first '&'.
        // p2 is the start of the string after the second '&'.
        
        // We need to find the '&' positions manually.
        const char* first_amp = NULL;
        const char* second_amp = NULL;
        
        const char* curr = s;
        while (*curr) {
            if (*curr == '&') {
                if (!first_amp) first_amp = curr;
                else if (!second_amp) {
                    second_amp = curr;
                    break;
                }
            }
            curr++;
        }

        // If strtok(buf, "&") is called:
        // p0 = buf
        // p1 = first_amp + 1
        // p2 = second_amp + 1
        
        // We use the same offsets as the baseline:
        u = fast_atoi(s + 2);
        if (first_amp) {
            s_val = fast_atoi(first_amp + 1 + 6);
            if (second_amp) {
                f = fast_atoi(second_amp + 1 + 5);
            }
        }

        uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
