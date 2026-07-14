#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        uint64_t v = 0;

        // Fixed format: %c%c%4u-%8X (Total 15 chars)
        // Indices: 0,1 (chars), 2,3,4,5 (digits), 6 ('-'), 7,8,9,10,11,12,13,14 (hex)
        // Check length first (must be exactly 15)
        if (row[15] == '\0') {
            int valid = 1;
            // Validate %4u (indices 2,3,4,5)
            for (int j = 2; j <= 5; ++j) {
                if (row[j] < '0' || row[j] > '9') {
                    valid = 0;
                    break;
                }
            }
            if (valid && row[6] == '-') {
                // Validate %8X (indices 7 to 14)
                for (int j = 7; j <= 14; ++j) {
                    char c = row[j];
                    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
                }
            }
        }

        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}