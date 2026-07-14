#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int parse_int(const char* s) {
    int res = 0;
    while (*s >= '0' && *s <= '9') {
        res = res * 10 + (*s - '0');
        s++;
    }
    return res;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        const char* p1 = strchr(row, '&');
        const char* p2 = strchr(p1 + 1, '&');
        
        int u = parse_int(row + 2);
        int s = parse_int(p1 + 1 + 6);
        int f = parse_int(p2 + 1 + 5);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}