#include "interface.h"
#include <stdint.h>

static inline int fast_parse_int(const char *p) {
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_MULT = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *curr = rows[i];
        int u = fast_parse_int(curr + 2);
        while (*curr != '&') curr++;
        curr++;
        int s = fast_parse_int(curr + 6);
        while (*curr != '&') curr++;
        curr++;
        int f = fast_parse_int(curr + 5);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= FNV_MULT;
    }
    return hash;
}