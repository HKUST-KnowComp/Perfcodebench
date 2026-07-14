#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t mult = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        if (*p == 'r') ++p;
        while ((unsigned)(*p - '0') <= 9) ++p;
        ++p;
        while ((unsigned)(*p - '0') <= 9) ++p;
        ++p;
        uint64_t b = 0;
        while ((unsigned)(*p - '0') <= 9) {
            b = b * 10 + (uint64_t)(*p - '0');
            ++p;
        }
        ++p;
        uint64_t c = 0;
        while ((unsigned)(*p - '0') <= 9) {
            c = c * 10 + (uint64_t)(*p - '0');
            ++p;
        }
        ++p;
        uint64_t v = b + c + 22;
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
            v += 29;
        }
        hash ^= (v + 1);
        hash *= mult;
    }
    return hash;
}