#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime  = 1099511628211ULL;
    uint64_t hash = fnv_offset;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char *)rows[i];

        /* fast reject if the row is not exactly 15 bytes long               */
        if (s[15] != 0) continue;

        /* parse format manually: "%c%c%4u-%8X"                             */
        /* we only need the two characters at positions 2 and 7              */
        /* positions:  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14                  */
        /*             ^ ^ ^       ^                                       */
        uint64_t v = (uint64_t)(s[2] + s[7]);

        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}