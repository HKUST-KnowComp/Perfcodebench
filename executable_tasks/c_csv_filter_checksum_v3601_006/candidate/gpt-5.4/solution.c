#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        /* Skip leading 'r' and id field up to first comma */
        ++p;
        while (*p != ',') ++p;
        ++p;

        /* Skip a field up to second comma */
        while (*p != ',') ++p;
        ++p;

        /* Parse b */
        int neg = 0;
        if (*p == '-') {
            neg = 1;
            ++p;
        }
        int b = 0;
        while ((unsigned)(*p - '0') < 10u) {
            b = b * 10 + (int)(*p - '0');
            ++p;
        }
        if (neg) b = -b;
        ++p; /* skip comma */

        /* Parse c */
        neg = 0;
        if (*p == '-') {
            neg = 1;
            ++p;
        }
        int c = 0;
        while ((unsigned)(*p - '0') < 10u) {
            c = c * 10 + (int)(*p - '0');
            ++p;
        }
        if (neg) c = -c;
        ++p; /* skip comma */

        uint64_t v = (uint64_t)(b + c + 25);

        /* Exact match for flag == "hot" */
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }

    return hash;
}
