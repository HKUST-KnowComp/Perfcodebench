#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;

        // Format: %c%c%4u-%8X (Total 15 chars)
        // Indices: 01 2345 6 789ABCDE
        // Check length and fixed separator '-'
        if (r[0] != '\0' && r[1] != '\0' && r[2] != '\0' && r[3] != '\0' && 
            r[4] != '\0' && r[5] != '\0' && r[6] == '-' && r[7] != '\0' && 
            r[8] != '\0' && r[9] != '\0' && r[10] != '\0' && r[11] != '\0' && 
            r[12] != '\0' && r[13] != '\0' && r[14] != '\0' && r[15] == '\0') {
            
            // Validate %4u (indices 2,3,4,5)
            int valid = is_digit(r[2]) && is_digit(r[3]) && is_digit(r[4]) && is_digit(r[5]);
            
            // Validate %8X (indices 7,8,9,10,11,12,13,14)
            if (valid) {
                valid = is_hex(r[7]) && is_hex(r[8]) && is_hex(r[9]) && is_hex(r[10]) &&
                        is_hex(r[11]) && is_hex(r[12]) && is_hex(r[13]) && is_hex(r[14]);
            }

            if (valid) {
                // Baseline logic: v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7])
                v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
            }
        }

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}