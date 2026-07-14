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

        // Fixed format check: length must be exactly 15
        // Format: %c%c%4u-%8X
        // Indices: 01 2345 6 78901234
        if (row[15] == '\0') {
            // Check if indices 2,3,4,5 are digits
            if (is_digit(row[2]) && is_digit(row[3]) && is_digit(row[4]) && is_digit(row[5])) {
                // Check if index 6 is '-'
                if (row[6] == '-') {
                    // Check if indices 7-14 are hex
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