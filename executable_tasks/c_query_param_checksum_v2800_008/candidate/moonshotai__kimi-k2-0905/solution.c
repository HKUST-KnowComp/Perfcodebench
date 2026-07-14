#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];

        /* ---- parse u=... ---- */
        uint64_t u = 0;
        s += 2;                     /* skip "u=" */
        while (*s >= '0' && *s <= '9') {
            u = u * 10U + (uint64_t)(*s++ - '0');
        }
        ++s;                        /* skip '&' */

        /* ---- parse s=search... ---- */
        uint64_t s_val = 0;
        s += 6;                     /* skip "s=search" */
        while (*s >= '0' && *s <= '9') {
            s_val = s_val * 10U + (uint64_t)(*s++ - '0');
        }
        ++s;                        /* skip '&' */

        /* ---- parse f=flag... ---- */
        uint64_t f = 0;
        s += 5;                     /* skip "f=flag" */
        while (*s >= '0' && *s <= '9') {
            f = f * 10U + (uint64_t)(*s++ - '0');
        }

        uint64_t v = ((u & 2047ULL) + s_val + f * 19ULL) + 1ULL;
        hash ^= v;
        hash *= prime;
    }
    return hash;
}