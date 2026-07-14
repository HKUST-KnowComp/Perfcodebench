#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static inline int is_hex(char c) {
    return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // Fixed format: %c%c%4u-%8X (Total 15 chars)
        // Indices: 0:c, 1:c, 2-5:4u, 6: '-', 7-14: 8X, 15: '\0'
        if (s[15] == '\0') {
            int matched = 1;
            // Check 4u part (indices 2, 3, 4, 5)
            if (!is_digit(s[2]) || !is_digit(s[3]) || !is_digit(s[4]) || !is_digit(s[5])) {
                matched = 0;
            } else if (s[6] != '-') {
                matched = 0;
            } else {
                // Check 8X part (indices 7 through 14)
                for (int j = 7; j < 15; ++j) {
                    if (!is_hex(s[j])) {
                        matched = 0;
                        break;
                    }
                }
            }

            if (matched) {
                v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
            }
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}