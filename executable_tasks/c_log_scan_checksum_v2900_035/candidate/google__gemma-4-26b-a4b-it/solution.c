#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // The requirement is: matched == 4 && strlen(rows[i]) == 15
        // Format: %c%c%4u-%8X
        // Indices: 0123456789ABCDE
        // Example: AA1234-ABCDEFGH (Wait, %4u is 4 digits, %8X is 8 hex)
        // Total length: 2 (chars) + 4 (digits) + 1 (hyphen) + 8 (hex) = 15
        
        // Check length 15 and hyphen position
        if (s[0] != '\0' && s[15] == '\0' && s[6] == '-') {
            // Validate digits for %4u (indices 2,3,4,5)
            if (s[2] >= '0' && s[2] <= '9' &&
                s[3] >= '0' && s[3] <= '9' &&
                s[4] >= '0' && s[4] <= '9' &&
                s[5] >= '0' && s[5] <= '9') {
                
                // Validate hex for %8X (indices 7,8,9,10,11,12,13,14)
                // We only need to ensure they are valid hex to satisfy sscanf == 4
                int valid_hex = 1;
                for (int j = 7; j < 15; ++j) {
                    char c = s[j];
                    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                        valid_hex = 0;
                        break;
                    }
                }

                if (valid_hex) {
                    v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}