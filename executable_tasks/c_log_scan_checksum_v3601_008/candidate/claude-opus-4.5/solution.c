#include "interface.h"

#include <stdint.h>

static inline int is_digit(unsigned char c) {
    return (unsigned)(c - '0') < 10u;
}

static inline int is_hex(unsigned char c) {
    return is_digit(c) || ((unsigned)(c - 'A') < 6u) || ((unsigned)(c - 'a') < 6u);
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char* r = (const unsigned char*)rows[i];
        uint64_t v = 0;
        
        /* Check length is exactly 15 and validate format: XX####-XXXXXXXX */
        if (r[0] && r[1] && r[2] && r[3] && r[4] && r[5] && r[6] && 
            r[7] && r[8] && r[9] && r[10] && r[11] && r[12] && r[13] && r[14] && !r[15] &&
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
