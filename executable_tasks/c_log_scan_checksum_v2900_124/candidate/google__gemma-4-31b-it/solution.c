#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int is_digit(char c) {
    return (unsigned char)c >= '0' && (unsigned char)c <= '9';
}

static inline int is_hex(char c) {
    return ((unsigned char)c >= '0' && (unsigned char)c <= '9') ||
           ((unsigned char)c >= 'A' && (unsigned char)c <= 'F') ||
           ((unsigned char)c >= 'a' && (unsigned char)c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        uint64_t v = 0;

        // Fixed format: %c%c%4u-%8X (Total 1+1+4+1+8 = 15 chars)
        // Check length first to avoid strlen
        if (row[15] == '\0') {
            // Validate %4u (indices 2, 3, 4, 5)
            if (is_digit(row[2]) && is_digit(row[3]) && is_digit(row[4]) && is_digit(row[5])) {
                // Validate separator (index 6)
                if (row[6] == '-') {
                    // Validate %8X (indices 7 to 14)
                    int hex_ok = 1;
                    for (int j = 7; j < 15; ++j) {
                        if (!is_hex(row[j])) {
                            hex_ok = 0;
                            break;
                        }
                    }
                    if (hex_ok) {
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