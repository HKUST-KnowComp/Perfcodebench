
#include "interface.h"
#include <stdint.h>
#include <string.h>

static inline uint64_t parse_u64(const char *p, const char *end)
{
    uint64_t v = 0;
    while (p < end && *p >= '0' && *p <= '9') {
        v = v * 10ULL + (uint64_t)(*p - '0');
        ++p;
    }
    return v;
}

uint64_t run(const char *const *rows, size_t row_count)
{
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];

        /* skip "r" and parse id */
        uint64_t id = 0;
        ++s;
        while (*s >= '0' && *s <= '9') {
            id = id * 10ULL + (uint64_t)(*s - '0');
            ++s;
        }
        ++s; /* ',' */

        /* parse a */
        uint64_t a = 0;
        while (*s >= '0' && *s <= '9') {
            a = a * 10ULL + (uint64_t)(*s - '0');
            ++s;
        }
        ++s; /* ',' */

        /* parse b */
        uint64_t b = 0;
        while (*s >= '0' && *s <= '9') {
            b = b * 10ULL + (uint64_t)(*s - '0');
            ++s;
        }
        ++s; /* ',' */

        /* parse c */
        uint64_t c = 0;
        while (*s >= '0' && *s <= '9') {
            c = c * 10ULL + (uint64_t)(*s - '0');
            ++s;
        }
        ++s; /* ',' */

        /* read flag (3 chars "hot" or 4 chars "cold") */
        uint64_t v = b + c + 8ULL;
        if (s[0] == 'h' && s[1] == 'o' && s[2] == 't') {
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}
