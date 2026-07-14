#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* p) {
    int val = 0;
    int sign = 1;
    while (*p == ' ') p++;
    if (*p == '-') { sign = -1; p++; }
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val * sign;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* ptr = rows[i];
        
        while (*ptr != '=') ptr++;
        int u = fast_atoi(ptr + 1);
        
        ptr++;
        while (*ptr != '=') ptr++;
        int s = fast_atoi(ptr + 1);
        
        ptr++;
        while (*ptr != '=') ptr++;
        int f = fast_atoi(ptr + 1);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}