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
        // This implies a format like 'AB1234-ABCDEFGH' (where 1234 is 4u and ABCDEFGH is 8X)
        // Wait, the format %c%c%4u-%8X is: char, char, 4-digit uint, '-', 8-digit hex.
        // Total length: 1 + 1 + 4 + 1 + 8 = 15.
        
        // Check length 15 and the '-' at index 6
        // We also need to ensure the characters are valid for the sscanf pattern to match.
        // However, the checksum only depends on rows[i][2] and rows[i][7].
        // Let's verify the structure to ensure 'matched == 4' and 'strlen == 15'.
        
        // Fast length check: check if null terminator is at index 15
        if (r[15] == '\0') {
            // Check '-' at index 6
            if (r[6] == '-') {
                // Check if the 4u part (indices 2,3,4,5) are digits
                if (r[2] >= '0' && r[2] <= '9' &&
                    r[3] >= '0' && r[3] <= '9' &&
                    r[4] >= '0' && r[4] <= '9' &&
                    r[5] >= '0' && r[5] <= '9') {
                    
                    // Check if the 8X part (indices 7-14) are hex
                    // We can do this more efficiently, but let's check the checksum requirement.
                    // The checksum only needs r[2] and r[7].
                    // We must ensure the whole string is valid hex for the 8X part to satisfy sscanf.
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
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}