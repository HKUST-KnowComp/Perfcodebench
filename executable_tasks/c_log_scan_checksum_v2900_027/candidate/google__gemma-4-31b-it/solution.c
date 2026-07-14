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

        // Fixed format check: %c%c%4u-%8X and length 15
        // Indices: 0:c, 1:c, 2-5:4u, 6:-, 7-14:8X
        // Total length must be exactly 15
        if (row[15] == '\0') {
            int valid = 1;
            // Check 4u part (indices 2, 3, 4, 5)
            if (!is_digit(row[2]) || !is_digit(row[3]) || !is_digit(row[4]) || !is_digit(row[5])) {
                valid = 0;
            } else if (row[6] != '-') {
                valid = 0;
            } else {
                // Check 8X part (indices 7 to 14)
                for (int j = 7; j < 15; ++j) {
                    if (!is_hex(row[j])) {
                        valid = 0;
                        break;
                    }
                }
            }

            if (valid) {
                v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
            }
        }

        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}