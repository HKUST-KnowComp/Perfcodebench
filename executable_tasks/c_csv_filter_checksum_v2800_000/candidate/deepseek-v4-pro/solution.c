#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    const uint64_t multiplier = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        ++p;                     // skip 'r'
        while (*p >= '0' && *p <= '9') ++p;  // skip id
        ++p;                     // skip ','
        while (*p >= '0' && *p <= '9') ++p;  // skip a
        ++p;                     // skip ','
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (int)(*p - '0');
            ++p;
        }
        ++p;                     // skip ','
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (int)(*p - '0');
            ++p;
        }
        ++p;                     // skip ','
        uint64_t v = (uint64_t)(b + c + 7);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0')
            v += 29ULL;
        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}