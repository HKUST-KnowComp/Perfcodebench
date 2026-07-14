#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* const row = rows[i];
        uint64_t v = 0;
        // Exact 15-byte length check equivalent to original strlen(row) ==15
        if (row[15] == '\0') {
            // Verify required '-' delimiter in fixed position, matching sscanf format
            if (row[6] == '-') {
                // Validate 4 decimal digits for %4u to parse successfully
                if ((row[2] >= '0' && row[2] <= '9') &&
                    (row[3] >= '0' && row[3] <= '9') &&
                    (row[4] >= '0' && row[4] <= '9') &&
                    (row[5] >= '0' && row[5] <= '9')) {
                    // Validate 8 hex digits for %8X to parse successfully
                    int hex_valid = 1;
                    for (int j = 7; j <= 14; ++j) {
                        const unsigned char c = (unsigned char)row[j];
                        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                            hex_valid = 0;
                            break;
                        }
                    }
                    if (hex_valid) {
                        // Exact same value calculation as original, preserves correctness
                        v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
                    }
                }
            }
        }
        // Identical FNV-1a hash update to maintain checksum correctness
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}