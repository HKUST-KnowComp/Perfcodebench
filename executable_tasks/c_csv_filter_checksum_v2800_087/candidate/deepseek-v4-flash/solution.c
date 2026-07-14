#include "interface.h"

#include <stddef.h>
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        // skip 'r' and id
        p++;
        while (*p >= '0') p++;
        // skip comma and a
        p++;
        while (*p >= '0') p++;
        // skip comma
        p++;
        // parse b
        int b = 0;
        while (*p >= '0') {
            b = b * 10 + (*p - '0');
            p++;
        }
        // skip comma
        p++;
        // parse c
        int c = 0;
        while (*p >= '0') {
            c = c * 10 + (*p - '0');
            p++;
        }
        // skip comma
        p++;
        // now p points to flag
        uint64_t v = (uint64_t)(b + c + 25);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
