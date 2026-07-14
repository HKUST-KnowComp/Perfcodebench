#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char *)rows[i];

        /* Fast reject if the 15-byte layout is violated */
        if (s[ 4] != '-' ||
            s[15] != '\0') {
            continue;
        }

        /* Extract the two required bytes and compute value */
        uint64_t v = (uint64_t)(s[2] + s[7]) + 1ULL;

        /* FNV-1a update */
        hash ^= v;
        hash *= prime;
    }
    return hash;
}