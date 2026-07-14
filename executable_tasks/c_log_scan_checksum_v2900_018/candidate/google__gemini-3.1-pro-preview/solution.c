#include "interface.h"
#include <stdint.h>
#include <stdbool.h>

static inline bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline bool is_xdigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline bool is_space(char c) {
    return c == ' ' || (c >= '\t' && c <= '\r');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // Check length exactly 15
        if (s[0] != '\0' && s[1] != '\0' && s[2] != '\0' && s[3] != '\0' && s[4] != '\0' && 
            s[5] != '\0' && s[6] != '\0' && s[7] != '\0' && s[8] != '\0' && s[9] != '\0' && 
            s[10] != '\0' && s[11] != '\0' && s[12] != '\0' && s[13] != '\0' && s[14] != '\0' && 
            s[15] == '\0') {
            
            int idx = 2;
            // sscanf %4u: skip leading whitespace, then read up to 4 digits
            while (idx < 6 && is_space(s[idx])) idx++;
            
            if (idx < 6 && is_digit(s[idx])) {
                while (idx < 6 && is_digit(s[idx])) idx++;
                
                // Next must be '-'
                if (s[idx] == '-') {
                    idx++;
                    // sscanf %8X: skip leading whitespace, then read up to 8 hex digits
                    while (idx < 15 && is_space(s[idx])) idx++;
                    
                    if (idx < 15 && is_xdigit(s[idx])) {
                        // Successfully matched all 4 components
                        v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
                    }
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}