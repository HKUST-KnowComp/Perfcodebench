#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char *)rows[i];

        /* fast reject anything whose 15th byte is not '\0' */
        if (s[15] != '\0') continue;

        /* validate fixed delimiters: s[2]=='-' && s[7]=='-' */
        if ((s[2] ^ '-') | (s[7] ^ '-')) continue;

        /* extract x = 4-digit decimal at s+3 */
        unsigned int x = (s[3]-'0') * 1000u
                       + (s[4]-'0') *  100u
                       + (s[5]-'0') *   10u
                       + (s[6]-'0');

        /* extract y = 8-digit hex at s+8 */
        uint32_t y = 0;
        for (int k = 0; k < 8; ++k) {
            unsigned char c = s[8+k];
            uint32_t n = c - '0';
            if (n > 9) n = (c & 0xdf) - 'A' + 10; /* uppercase */
            y = (y << 4) | n;
        }

        uint64_t v = (uint64_t)(s[0] + s[1] + x + y);
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}