#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline const char* parse_int(const char* p, int* out) {
    int neg = 0;
    if (*p == '-') {
        neg = 1;
        ++p;
    } else if (*p == '+') {
        ++p;
    }
    int x = 0;
    while (*p >= '0' && *p <= '9') {
        x = x * 10 + (*p++ - '0');
    }
    *out = neg ? -x : x;
    return p;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        int u, s, f;
        p = parse_int(p + 2, &u);
        while (*p != '&') ++p;
        p = parse_int(p + 7, &s);
        while (*p != '&') ++p;
        p = parse_int(p + 6, &f);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}