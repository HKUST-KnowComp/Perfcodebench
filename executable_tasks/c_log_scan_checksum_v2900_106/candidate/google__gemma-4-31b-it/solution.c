#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        uint64_t v = 0;

        // Fixed format: %c%c%4u-%8X (Total 1+1+4+1+8 = 15 chars)
        // Check length first (fast path)
        // We check indices 0 to 14. If row[15] is not '\0', it's not length 15.
        if (row[15] == '\0') {
            // Validate %4u (indices 2, 3, 4, 5)
            if (is_digit(row[2]) && is_digit(row[3]) && is_digit(row[4]) && is_digit(row[5])) {
                // Validate '-' (index 6)
                if (row[6] == '-') {
                    // Validate %8X (indices 7, 8, 9, 10, 11, 12, 13, 14)
                    if (is_hex(row[7]) && is_hex(row[8]) && is_hex(row[9]) && is_hex(row[10]) &&
                        is_hex(row[11]) && is_hex(row[12]) && is_hex(row[13]) && is_hex(row[14])) {
                        
                        v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
                    }
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}