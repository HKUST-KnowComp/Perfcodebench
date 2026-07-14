#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t m = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *r = (const unsigned char *)rows[i];

        /* Check fixed format: positions 2 and 7 must be '-' and the string must be 15 chars */
        if (r[2] == '-' && r[7] == '-' && r[14] != 0) {
            uint64_t v = (uint64_t)(r[0] + r[5]) + 1ULL;
            hash ^= v;
            hash *= m;
        }
    }
    return hash;
}