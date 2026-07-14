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
        const char* first_amp = strchr(row, '&');
        const char* second_amp = strchr(first_amp + 1, '&');
        
        const char* part0 = row;
        const char* num0 = part0 + 2;
        int u = parse_int(num0);
        
        const char* part1 = first_amp + 1;
        const char* num1 = part1 + 6;
        int s = parse_int(num1);
        
        const char* part2 = second_amp + 1;
        const char* num2 = part2 + 5;
        int f = parse_int(num2);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}