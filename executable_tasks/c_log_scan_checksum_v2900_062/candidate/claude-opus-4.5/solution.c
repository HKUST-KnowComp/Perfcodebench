#include "interface.h"

#include <stdint.h>

static inline int is_digit(unsigned char c) {
    return (c >= '0') & (c <= '9');
}

static inline int is_hex(unsigned char c) {
    return ((c >= '0') & (c <= '9')) | ((c >= 'A') & (c <= 'F')) | ((c >= 'a') & (c <= 'f'));
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char* r = (const unsigned char*)rows[i];
        uint64_t v = 0;
        
        /* Format: CCdddd-XXXXXXXX (15 chars)
         * positions 0,1: any char (p0, p1)
         * positions 2-5: 4 decimal digits
         * position 6: dash
         * positions 7-14: 8 hex digits
         * position 15: null terminator
         */
        
        if (r[15] == '\0' &&
            is_digit(r[2]) && is_digit(r[3]) && is_digit(r[4]) && is_digit(r[5]) &&
            r[6] == '-' &&
            is_hex(r[7]) && is_hex(r[8]) && is_hex(r[9]) && is_hex(r[10]) &&
            is_hex(r[11]) && is_hex(r[12]) && is_hex(r[13]) && is_hex(r[14])) {
            v = (uint64_t)(r[2] + r[7]);
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}