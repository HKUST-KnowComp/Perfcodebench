#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        int u = 0, s = 0, f = 0;

        // skip "u="
        p += 2;
        while (*p >= '0' && *p <= '9') {
            u = u * 10 + (*p - '0');
            ++p;
        }
        // skip '&s='
        p += 3;
        while (*p >= '0' && *p <= '9') {
            s = s * 10 + (*p - '0');
            ++p;
        }
        // skip '&f='
        p += 3;
        while (*p >= '0' && *p <= '9') {
            f = f * 10 + (*p - '0');
            ++p;
        }

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}