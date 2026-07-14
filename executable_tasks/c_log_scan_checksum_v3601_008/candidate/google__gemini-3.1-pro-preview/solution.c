#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 0;

        // Check fixed length 15: r[0..14] must be non-null, r[15] must be null.
        // We check characters in an order that allows early exit.
        if (r[0] && r[1] && r[2] && r[3] && r[4] && r[5] && 
            r[6] == '-' && 
            r[7] && r[8] && r[9] && r[10] && r[11] && r[12] && r[13] && r[14] && 
            r[15] == '\0') {
            
            // Validate decimal field %4u (r[2..5])
            // sscanf %4u skips leading whitespace, but fixed-format usually implies digits.
            // The baseline sscanf returns 4 if it matches the pattern.
            if (is_digit(r[2]) && is_digit(r[3]) && is_digit(r[4]) && is_digit(r[5]) &&
                is_hex(r[7]) && is_hex(r[8]) && is_hex(r[9]) && is_hex(r[10]) &&
                is_hex(r[11]) && is_hex(r[12]) && is_hex(r[13]) && is_hex(r[14])) {
                v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
            }
        }

        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}
