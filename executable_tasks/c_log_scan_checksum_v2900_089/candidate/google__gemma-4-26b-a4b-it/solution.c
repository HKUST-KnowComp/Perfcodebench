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
        // Example: AA1234-ABCDEFGH (Wait, %4u is 4 digits, %8X is 8 hex)
        // Total length: 2 (chars) + 4 (digits) + 1 (dash) + 8 (hex) = 15
        
        // Fast length check: check if the 15th char is null terminator
        // and ensure the string isn't shorter than 15.
        if (r[15] == '\0') {
            // Validate the '-' at index 6
            if (r[6] == '-') {
                // Validate digits at 2,3,4,5 and hex at 7-14
                // However, the checksum only depends on r[2] and r[7].
                // The sscanf requirement 'matched == 4' implies the pattern must hold.
                // We must ensure r[2..5] are digits and r[7..14] are hex.
                
                int valid = 1;
                // Check digits 2,3,4,5
                for (int j = 2; j <= 5; ++j) {
                    if (r[j] < '0' || r[j] > '9') { valid = 0; break; }
                }
                if (valid) {
                    // Check hex 7-14
                    for (int j = 7; j <= 14; ++j) {
                        char c = r[j];
                        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                            valid = 0; break;
                        }
                    }
                }

                if (valid) {
                    v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}