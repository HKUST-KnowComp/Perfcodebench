#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char *)rows[i];

        /* Fast 15-byte length check: simply look at the 16th byte. */
        if (s[15] != '\0') continue;

        /* Parse x (4 decimal digits starting at s[2]) */
        unsigned int x = (s[2] - '0') * 1000 +
                         (s[3] - '0') * 100  +
                         (s[4] - '0') * 10   +
                         (s[5] - '0');

        /* Parse y (8 hex digits starting at s[7]) */
        unsigned int y = 0;
        for (int k = 7; k < 15; ++k) {
            unsigned char c = s[k];
            y = (y << 4) | (c <= '9' ? c - '0' : (c & 0xdf) - 'A' + 10);
        }

        /* Ensure the two fixed separators are present. */
        if (s[6] != '-') continue;

        uint64_t v = (uint64_t)(s[2] + s[7]);
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}