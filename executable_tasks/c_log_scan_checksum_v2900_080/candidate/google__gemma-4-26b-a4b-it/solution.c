#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // The requirement is: matched == 4 AND strlen == 15
        // Format: %c%c%4u-%8X
        // Indices: 0123456789ABCDE
        // Example: AB1234-ABCDEFGH (Wait, the format is %c%c%4u-%8X)
        // %c (1) + %c (1) + %4u (4) + '-' (1) + %8X (8) = 15 chars
        
        // Check length 15 by verifying the null terminator is at index 15
        // and checking the structure manually to avoid sscanf overhead.
        if (s[15] == '\0') {
            // Check if the '-' is at the correct position (index 7)
            if (s[7] == '-') {
                // Check if the 4u part (indices 2,3,4,5) are digits
                if (s[2] >= '0' && s[2] <= '9' &&
                    s[3] >= '0' && s[3] <= '9' &&
                    s[4] >= '0' && s[4] <= '9' &&
                    s[5] >= '0' && s[5] <= '9') {
                    
                    // Check if the 8X part (indices 8-15) are hex digits
                    // Note: s[15] is '\0', so we check 8 to 14
                    // Wait, 8X is 8 chars. 8,9,10,11,12,13,14,15? 
                    // If strlen is 15, indices are 0-14. 
                    // Let's re-verify: 2 chars + 4 digits + 1 dash + 8 hex = 15.
                    // Indices: 0,1 (chars), 2,3,4,5 (digits), 6 (dash), 7,8,9,10,11,12,13,14 (hex)
                    // The original code used sscanf(rows[i], "%c%c%4u-%8X", ...)
                    // and checked strlen == 15. 
                    // If strlen is 15, indices are 0 to 14.
                    // Let's re-calculate: 2 + 4 + 1 + 8 = 15. Correct.
                    // The dash is at index 6. The hex starts at index 7.
                    // The original code's sscanf: %c(0), %c(1), %4u(2,3,4,5), -(6), %8X(7,8,9,10,11,12,13,14)
                    // The original code's v calculation: (unsigned char)rows[i][2] + (unsigned char)rows[i][7]
                    
                    // Validate hex digits at 7-14
                    int is_hex = 1;
                    for (int j = 7; j < 15; ++j) {
                        char c = s[j];
                        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                            is_hex = 0;
                            break;
                        }
                    }

                    if (is_hex) {
                        v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
                    }
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}
