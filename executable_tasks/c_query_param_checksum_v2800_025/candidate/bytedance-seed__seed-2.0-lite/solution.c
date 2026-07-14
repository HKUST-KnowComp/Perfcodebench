#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline int fast_parse_int(const char* start, const char* end) {
    int val = 0;
    while (start < end) {
        val = val * 10 + (*start++ - '0');
    }
    return val;
}

static inline int fast_parse_int_null(const char* start) {
    int val = 0;
    while (*start) {
        val = val * 10 + (*start++ - '0');
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        const char* amp1 = strchr(row, '&');
        const char* amp2 = strchr(amp1 + 1, '&');
        int u = fast_parse_int(row + 2, amp1);
        int s = fast_parse_int(amp1 + 7, amp2);
        int f = fast_parse_int_null(amp2 + 6);
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}