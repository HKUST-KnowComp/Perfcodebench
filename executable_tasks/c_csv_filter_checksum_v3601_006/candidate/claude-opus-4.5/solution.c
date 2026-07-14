#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        /* skip 'r' prefix */
        if (*p == 'r') ++p;
        /* skip id field */
        while (*p && *p != ',') ++p;
        if (*p == ',') ++p;
        /* skip a field */
        while (*p && *p != ',') ++p;
        if (*p == ',') ++p;
        /* parse b */
        int neg = 0;
        if (*p == '-') { neg = 1; ++p; }
        int b = 0;
        while (*p >= '0' && *p <= '9') { b = b * 10 + (*p - '0'); ++p; }
        if (neg) b = -b;
        if (*p == ',') ++p;
        /* parse c */
        neg = 0;
        if (*p == '-') { neg = 1; ++p; }
        int c = 0;
        while (*p >= '0' && *p <= '9') { c = c * 10 + (*p - '0'); ++p; }
        if (neg) c = -c;
        if (*p == ',') ++p;
        /* check flag */
        uint64_t v = (uint64_t)(b + c + 25);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ',' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
