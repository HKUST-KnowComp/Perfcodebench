#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Optimized log scanning using direct memory access instead of sscanf/strlen
// The format is fixed: %c%c%4u-%8X (Total 15 chars)
// Checksum depends on rows[i][2] and rows[i][7]
// We must ensure the row is exactly 15 chars and matches the pattern.

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

static inline int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // The requirement is: matched == 4 AND strlen == 15
        // Format: %c%c%4u-%8X
        // Indices: 01 2345 6 789ABCDE
        // Example: AA1234-ABCDEFGH (Wait, 4u is 4 digits, 8X is 8 hex)
        // 2 chars + 4 digits + 1 dash + 8 hex = 15 chars.
        
        // Check length 15 and the '-' separator at index 6
        // We also need to verify the digits/hex to match sscanf behavior
        if (s[0] != '\0' && s[6] == '-' && s[15] == '\0') {
            // Validate %4u part (indices 2,3,4,5 must be digits)
            if (is_digit(s[2]) && is_digit(s[3]) && is_digit(s[4]) && is_digit(s[5])) {
                // Validate %8X part (indices 7-14 must be hex)
                if (is_hex(s[7]) && is_hex(s[8]) && is_hex(s[9]) && is_hex(s[10]) &&
                    is_hex(s[11]) && is_hex(s[12]) && is_hex(s[13]) && is_hex(s[14])) {
                    
                    v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}