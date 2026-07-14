#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];
        /* fast rejection: length must be 15 and fixed delimiters present */
        uint64_t header;
        memcpy(&header, s, 8);
        if ((header & 0xFFFFFFFFFFFFULL) != 0x2D343030ULL)   /* "-400" in little-endian */
            continue;
        if (s[13] != 'X')
            continue;

        uint64_t v = ((uint8_t)s[2]) + ((uint8_t)s[7]);
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}