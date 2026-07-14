#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Performance-optimized log scanner.
 * Replaces sscanf and strlen with a manual fixed-format check.
 * The format "%c%c%4u-%8X" with strlen == 15 implies:
 * - rows[i][0], rows[i][1] are any chars.
 * - rows[i][2..5] are digits (0-9).
 * - rows[i][6] is '-'.
 * - rows[i][7..14] are hex digits (0-9, A-F, a-f).
 */

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;

        // Check length 15 and the '-' separator at index 6
        // We check r[15] == 0 to ensure strlen is exactly 15.
        if (r[0] != '\0' && r[1] != '\0' && r[2] != '\0' && r[3] != '\0' && 
            r[4] != '\0' && r[5] != '\0' && r[6] == '-' && r[7] != '\0' && 
            r[8] != '\0' && r[9] != '\0' && r[10] != '\0' && r[11] != '\0' && 
            r[12] != '\0' && r[13] != '\0' && r[14] != '\0' && r[15] == '\0') {
            
            // Validate %4u (indices 2, 3, 4, 5 must be digits)
            int is_digit = (r[2] >= '0' && r[2] <= '9') &&
                           (r[3] >= '0' && r[3] <= '9') &&
                           (r[4] >= '0' && r[4] <= '9') &&
                           (r[5] >= '0' && r[5] <= '9');

            if (is_digit) {
                // Validate %8X (indices 7 to 14 must be hex digits)
                int is_hex = 1;
                for (int j = 7; j <= 14; ++j) {
                    unsigned char c = (unsigned char)r[j];
                    if (!((c >= '0' && c <= '9') || 
                          (c >= 'A' && c <= 'F') || 
                          (c >= 'a' && c <= 'f'))) {
                        is_hex = 0;
                        break;
                    }
                }

                if (is_hex) {
                    v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}
