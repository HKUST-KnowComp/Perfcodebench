#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;
        
        // The original code requires: 
        // 1. sscanf matched 4 items (%c%c%4u-%8X)
        // 2. strlen(rows[i]) == 15
        // This implies a format like: AB1234-ABCDEFGH (where AB is 2 chars, 1234 is 4 digits, - is 1, ABCDEFGH is 8 hex)
        // Total length: 2 + 4 + 1 + 8 = 15.
        
        // Check length 15 by checking for null terminator at index 15
        // and validating the '-' at index 6.
        if (r[6] == '-' && r[15] == '\0') {
            // Validate digits for %4u (indices 2,3,4,5)
            if (r[2] >= '0' && r[2] <= '9' &&
                r[3] >= '0' && r[3] <= '9' &&
                r[4] >= '0' && r[4] <= '9' &&
                r[5] >= '0' && r[5] <= '9') {
                
                // Validate hex for %8X (indices 7,8,9,10,11,12,13,14)
                // Note: sscanf %X is case-insensitive, but the original code uses %8X.
                // We must ensure all 8 characters are valid hex digits.
                int valid_hex = 1;
                for (int j = 7; j < 15; ++j) {
                    char c = r[j];
                    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                        valid_hex = 0;
                        break;
                    }
                }

                if (valid_hex) {
                    v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}