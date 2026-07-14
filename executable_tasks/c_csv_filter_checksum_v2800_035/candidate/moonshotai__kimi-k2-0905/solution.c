#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];

        /* Skip "r" and the first number (id) */
        while (*s != ',') ++s; ++s;            /* past first comma */
        while (*s != ',') ++s; ++s;            /* past second comma */

        /* Parse b */
        int b = 0;
        int c = 0;
        while (*s != ',') {
            b = b * 10 + (*s - '0');
            ++s;
        }
        ++s;                                   /* skip comma */

        /* Parse c */
        while (*s != ',') {
            c = c * 10 + (*s - '0');
            ++s;
        }
        ++s;                                   /* skip comma */

        /* Check flag: "hot" is 3 bytes, guaranteed non-empty */
        int hot = 0;
        if (s[0] == 'h' && s[1] == 'o' && s[2] == 't') {
            hot = 29;
        }

        uint64_t v = (uint64_t)(b + c + 19) + (uint64_t)hot;
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}