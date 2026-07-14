#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Optimized log scanning using direct memory access and manual validation.
// The format is fixed: %c%c%4u-%8X (Total 15 chars)
// Checksum depends on rows[i][2] and rows[i][7] if valid.

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // Fast length and format validation for 15-character fixed format
        // Format: [0][1][2][3][4][5][6][7][8][9][10][11][12][13][14]
        // Pattern: %c %c %d %d %d %d - %X %X %X %X %X %X %X %X
        // Based on sscanf("%c%c%4u-%8X"), we need to ensure:
        // 1. Length is exactly 15 (including null terminator at index 15)
        // 2. Indices 6 is '-'
        // 3. Indices 2,3,4,5 are digits
        // 4. Indices 7,8,9,10,11,12,13,14 are hex digits

        if (s[0] != '\0' && s[15] == '\0') {
            // Check hyphen at index 6
            if (s[6] == '-') {
                // Check digits at 2,3,4,5
                if (s[2] >= '0' && s[2] <= '9' &&
                    s[3] >= '0' && s[3] <= '9' &&
                    s[4] >= '0' && s[4] <= '9' &&
                    s[5] >= '0' && s[5] <= '9') {
                    
                    // Check hex at 7-14
                    int hex_ok = 1;
                    for (int j = 7; j < 15; ++j) {
                        char c = s[j];
                        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                            hex_ok = 0;
                            break;
                        }
                    }

                    if (hex_ok) {
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