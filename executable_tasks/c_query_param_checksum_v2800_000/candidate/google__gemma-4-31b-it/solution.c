#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* p) {
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        // Skip "u="
        p += 2;
        int u = fast_atoi(p);

        // Find next '&' and skip "s="
        while (*p != '&') p++;
        p += 3; // skip "&s="
        int s = fast_atoi(p);

        // Find next '&' and skip "f="
        while (*p != '&') p++;
        p += 3; // skip "&f="
        int f = fast_atoi(p);

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}