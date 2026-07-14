#include "interface.h"

#include <stdint.h>

static inline uint64_t u64_hash(uint64_t h, uint64_t v) {
    h ^= (v + 1ULL);
    h *= 1099511628211ULL;
    return h;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];

        /* skip "r" */
        ++p;

        /* id */
        int id = 0;
        while (*p >= '0' && *p <= '9') {
            id = id * 10 + (*p - '0');
            ++p;
        }
        ++p; /* comma */

        /* a */
        int a = 0;
        while (*p >= '0' && *p <= '9') {
            a = a * 10 + (*p - '0');
            ++p;
        }
        ++p; /* comma */

        /* b */
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            ++p;
        }
        ++p; /* comma */

        /* c */
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            ++p;
        }
        ++p; /* comma */

        /* flag */
        int is_hot = 0;
        if (*p == 'h' && *(p + 1) == 'o' && *(p + 2) == 't') {
            is_hot = 1;
        }

        uint64_t v = (uint64_t)(b + c + 13);
        if (is_hot) v += 29ULL;
        hash = u64_hash(hash, v);
    }
    return hash;
}