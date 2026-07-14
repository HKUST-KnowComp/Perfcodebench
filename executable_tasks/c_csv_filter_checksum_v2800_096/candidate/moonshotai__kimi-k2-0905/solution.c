#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count)
{
    const uint64_t FNV_offset = 1469598103934665603ULL;
    const uint64_t FNV_prime  = 1099511628211ULL;

    uint64_t hash = FNV_offset;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];

        /* skip "r" and parse id */
        uint64_t id = 0;
        ++s;                         /* skip 'r' */
        while (*s >= '0' && *s <= '9') {
            id = id * 10 + (*s - '0');
            ++s;
        }
        ++s;                         /* skip ',' */

        /* parse a */
        uint64_t a = 0;
        while (*s >= '0' && *s <= '9') {
            a = a * 10 + (*s - '0');
            ++s;
        }
        ++s;                         /* skip ',' */

        /* parse b */
        uint64_t b = 0;
        while (*s >= '0' && *s <= '9') {
            b = b * 10 + (*s - '0');
            ++s;
        }
        ++s;                         /* skip ',' */

        /* parse c */
        uint64_t c = 0;
        while (*s >= '0' && *s <= '9') {
            c = c * 10 + (*s - '0');
            ++s;
        }
        ++s;                         /* skip ',' */

        /* flag: only need first char to decide "hot" vs others */
        uint64_t flag_bonus = 0;
        if (*s == 'h')           /* "hot" starts with 'h' */
            flag_bonus = 29;

        uint64_t v = b + c + 11 + flag_bonus;

        hash ^= (v + 1ULL);
        hash *= FNV_prime;
    }

    return hash;
}