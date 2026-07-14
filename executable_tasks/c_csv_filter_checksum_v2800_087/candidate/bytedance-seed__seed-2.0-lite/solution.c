#include "interface.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static inline const char* parse_int(const char* p, int* out) {
    int res = 0;
    while (*p >= '0' && *p <= '9') {
        res = res * 10 + (*p - '0');
        p++;
    }
    *out = res;
    if (*p == ',') p++;
    return p;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        int id, a, b, c;
        p++;
        p = parse_int(p, &id);
        p = parse_int(p, &a);
        p = parse_int(p, &b);
        p = parse_int(p, &c);
        int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ','));
        uint64_t v = (uint64_t)(b + c + 25);
        if (is_hot) v += 29ULL;
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}