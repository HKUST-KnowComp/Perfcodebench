#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int parse_until(const char *restrict p, char delim, const char **restrict out_next) {
    int val = 0;
    while (*p != delim && *p) {
        val = val * 10 + (*p - '0');
        p++;
    }
    *out_next = p + 1;
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const char *next;
    for (size_t i = 0; i < row_count; ++i) {
        const char *ptr = rows[i];
        int u = parse_until(ptr + 2, '&', &next);
        ptr = next;
        int s = parse_until(ptr + 6, '&', &next);
        ptr = next;
        int f = parse_until(ptr + 5, '\0', &next);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}