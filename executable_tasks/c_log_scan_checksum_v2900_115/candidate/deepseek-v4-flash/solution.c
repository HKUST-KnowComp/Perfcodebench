#include "interface.h"
#include <stdint.h>

static inline int is_hex(unsigned char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        // Check length exactly 15 (null terminator at index 15, not before)
        if (r[14] == '\0' || r[15] != '\0') {
            hash ^= 1ULL;
            hash *= 1099511628211ULL;
            continue;
        }
        // Validate decimal digits at positions 2-5
        if (r[2] < '0' || r[2] > '9' || r[3] < '0' || r[3] > '9' ||
            r[4] < '0' || r[4] > '9' || r[5] < '0' || r[5] > '9') {
            hash ^= 1ULL;
            hash *= 1099511628211ULL;
            continue;
        }
        // Validate hyphen at position 6
        if (r[6] != '-') {
            hash ^= 1ULL;
            hash *= 1099511628211ULL;
            continue;
        }
        // Validate hex digits at positions 7-14
        if (!is_hex((unsigned char)r[7]) || !is_hex((unsigned char)r[8]) ||
            !is_hex((unsigned char)r[9]) || !is_hex((unsigned char)r[10]) ||
            !is_hex((unsigned char)r[11]) || !is_hex((unsigned char)r[12]) ||
            !is_hex((unsigned char)r[13]) || !is_hex((unsigned char)r[14])) {
            hash ^= 1ULL;
            hash *= 1099511628211ULL;
            continue;
        }
        // Valid row: compute v and continue hash
        uint64_t v = (unsigned char)r[2] + (unsigned char)r[7];
        hash ^= v + 1ULL;
        hash *= 1099511628211ULL;
    }
    return hash;
}
