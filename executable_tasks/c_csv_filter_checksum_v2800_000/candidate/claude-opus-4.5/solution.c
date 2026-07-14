#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        /* skip 'r' */
        ++p;
        /* skip id (digits until comma) */
        while (*p != ',') ++p;
        ++p;
        /* skip a (digits until comma) */
        while (*p != ',') ++p;
        ++p;
        /* parse b */
        int neg = 0;
        if (*p == '-') { neg = 1; ++p; }
        int b = 0;
        while (*p >= '0' && *p <= '9') { b = b * 10 + (*p - '0'); ++p; }
        if (neg) b = -b;
        ++p; /* skip comma */
        /* parse c */
        neg = 0;
        if (*p == '-') { neg = 1; ++p; }
        int c = 0;
        while (*p >= '0' && *p <= '9') { c = c * 10 + (*p - '0'); ++p; }
        if (neg) c = -c;
        ++p; /* skip comma */
        /* check flag == "hot" */
        uint64_t v = (uint64_t)(b + c + 7);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ',' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
