#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int parse_int(const char* start) {
    int res = 0;
    while (*start >= '0' && *start <= '9') {
        res = res * 10 + (*start - '0');
        start++;
    }
    return res;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        const char* first_amp = strchr(row, '&');
        const char* second_amp = strchr(first_amp + 1, '&');
        
        int u = parse_int(row + 2);
        int s = parse_int(first_amp + 7);
        int f = parse_int(second_amp + 6);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}