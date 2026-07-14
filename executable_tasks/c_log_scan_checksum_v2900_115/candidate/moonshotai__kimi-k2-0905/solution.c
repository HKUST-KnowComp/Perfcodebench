#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count)
{
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *p = (const unsigned char *)rows[i];

        /* 16-byte load: 15 ASCII chars + terminating 0 */
        uint64_t v0, v1;
        memcpy(&v0, p, 8);
        memcpy(&v1, p + 8, 8);

        /* validate fixed positions in one go */
        const uint64_t m0 = 0x000000FF000000FF;  /* pos 0,1 */
        const uint64_t m1 = 0x000000000000FF00;  /* pos 7 */
        const uint64_t m2 = 0x00000000000000FF;  /* pos 14 */
        uint64_t bad = (v0 & m0) | (v1 & m1) | (v2 & m2);
        bad |= (p[2] != '-') | (p[7] != '-');

        uint64_t val = 0;
        if (__builtin_expect(bad == 0, 1))
            val = (uint64_t)(p[2] + p[7]);

        hash ^= (val + 1);
        hash *= prime;
    }
    return hash;
}