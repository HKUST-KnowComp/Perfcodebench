#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Helper to convert hex char to int
static inline int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // The requirement is: matched == 4 AND strlen == 15
        // Format: %c%c%4u-%8X
        // Indices: 01 2345 6 789ABCDE
        // Example: AA1234-ABCDEF12 (Wait, the format says %4u-%8X)
        // Let's re-examine the original logic:
        // sscanf(rows[i], "%c%c%4u-%8X", &p0, &p1, &x, &y)
        // p0: index 0, p1: index 1, x: index 2-5, '-': index 6, y: index 7-14
        // Total length: 15
        
        // Check length implicitly by checking if index 14 exists and index 15 is null
        // However, the original code uses strlen(rows[i]) == 15.
        // We must ensure the string is exactly 15 chars.
        
        // Fast check for length 15 and the '-' at index 6
        if (s[6] == '-' && s[15] == '\0') {
            // Validate the %4u part (indices 2,3,4,5 must be digits)
            if (s[2] >= '0' && s[2] <= '9' &&
                s[3] >= '0' && s[3] <= '9' &&
                s[4] >= '0' && s[4] <= '9' &&
                s[5] >= '0' && s[5] <= '9') {
                
                // Validate the %8X part (indices 7-14 must be hex)
                int valid_hex = 1;
                for (int j = 7; j < 15; ++j) {
                    if (hex_to_int(s[j]) == -1) {
                        valid_hex = 0;
                        break;
                    }
                }

                if (valid_hex) {
                    // If all conditions met, v = (uint64_t)((unsigned char)rows[i][2] + (unsigned char)rows[i][7])
                    v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}