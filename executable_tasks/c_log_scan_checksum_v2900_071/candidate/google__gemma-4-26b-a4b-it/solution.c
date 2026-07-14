#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;
        
        // The requirement is: matched == 4 AND strlen == 15
        // Format: %c%c%4u-%8X
        // Indices: 0123456789ABCDE
        // Example: AA1234-ABCDEFGH (Wait, 4u is 4 digits, 8X is 8 hex)
        // Let's re-examine the baseline: %c%c%4u-%8X
        // %c (1) + %c (1) + %4u (4) + '-' (1) + %8X (8) = 15 characters.
        
        // Fast length check: check if the 15th char is null terminator
        // and ensure the string isn't shorter than 15.
        if (r[15] == '\0') {
            // Validate the '-' at index 6
            if (r[6] == '-') {
                // Validate that the 4u part (indices 2,3,4,5) are digits
                if (r[2] >= '0' && r[2] <= '9' &&
                    r[3] >= '0' && r[3] <= '9' &&
                    r[4] >= '0' && r[4] <= '9' &&
                    r[5] >= '0' && r[5] <= '9') {
                    
                    // Validate that the 8X part (indices 7-14) are hex
                    // We check all 8 to ensure sscanf would have matched 4 items
                    int hex_ok = 1;
                    for (int j = 7; j < 15; ++j) {
                        char c = r[j];
                        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                            hex_ok = 0;
                            break;
                        }
                    }

                    if (hex_ok) {
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