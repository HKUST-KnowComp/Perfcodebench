#include "interface.h"
#include <stdint.h>
#include <stdbool.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        uint64_t v = 0;
        // Early fast checks for core fixed format requirements
        if (row[6] == '-' && row[15] == '\0') {
            bool is_valid = true;
            // Validate 4-digit numeric field starting at index 2
            is_valid &= (row[2] >= '0' && row[2] <= '9');
            is_valid &= (row[3] >= '0' && row[3] <= '9');
            is_valid &= (row[4] >= '0' && row[4] <= '9');
            is_valid &= (row[5] >= '0' && row[5] <= '9');
            // Validate 8-character hex field spanning indices 7-14
            for (int j = 7; j <= 14; ++j) {
                const char c = row[j];
                const bool is_hex = (c >= '0' && c <= '9') || 
                                    (c >= 'A' && c <= 'F') || 
                                    (c >= 'a' && c <= 'f');
                is_valid &= is_hex;
            }
            if (is_valid) {
                v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
            }
        }
        // Exact same FNV-1a hash update as original implementation
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}