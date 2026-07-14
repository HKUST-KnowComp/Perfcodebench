#include "interface.h"

#include <stdint.h>

static inline int is_digit(unsigned char c) {
    return c >= '0' && c <= '9';
}

static inline int is_hex(unsigned char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char* r = (const unsigned char*)rows[i];
        uint64_t v = 0;
        
        /* Check length is exactly 15 and validate format: XX9999-XXXXXXXX */
        if (r[0] && r[1] && r[2] && r[3] && r[4] && r[5] && r[6] &&
            r[7] && r[8] && r[9] && r[10] && r[11] && r[12] && r[13] && r[14] && !r[15]) {
            
            /* Validate: positions 2-5 are digits, position 6 is dash, positions 7-14 are hex */
            if (is_digit(r[2]) && is_digit(r[3]) && is_digit(r[4]) && is_digit(r[5]) &&
                r[6] == '-' &&
                is_hex(r[7]) && is_hex(r[8]) && is_hex(r[9]) && is_hex(r[10]) &&
                is_hex(r[11]) && is_hex(r[12]) && is_hex(r[13]) && is_hex(r[14])) {
                
                v = (uint64_t)(r[2] + r[7]);
            }
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}
