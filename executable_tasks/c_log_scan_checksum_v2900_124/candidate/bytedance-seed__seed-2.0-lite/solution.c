#include "interface.h"
#include <stdint.h>
#include <stdio.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    #define IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;
        // Validate original matching conditions with safe, fast inline checks
        if (r[14] != '\0' && r[15] == '\0' && r[6] == '-') {
            // Verify 4-digit sequence at offsets 2-5 (matches %4u format requirement)
            if ((r[2] >= '0' && r[2] <= '9') &&
                (r[3] >= '0' && r[3] <= '9') &&
                (r[4] >= '0' && r[4] <= '9') &&
                (r[5] >= '0' && r[5] <= '9')) {
                // Verify 8-hex sequence at offsets 7-14 (matches %8X format requirement)
                if (IS_HEX(r[7]) && IS_HEX(r[8]) && IS_HEX(r[9]) && IS_HEX(r[10]) &&
                    IS_HEX(r[11]) && IS_HEX(r[12]) && IS_HEX(r[13]) && IS_HEX(r[14])) {
                    v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
                }
            }
        }
        // Exact original hash update logic preserved
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    #undef IS_HEX
    return hash;
}