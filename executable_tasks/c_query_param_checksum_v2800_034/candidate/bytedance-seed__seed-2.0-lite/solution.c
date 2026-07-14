#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int parse_decimal(const char **ptr) {
    int val = 0;
    while (**ptr >= '0' && **ptr <= '9') {
        val = val * 10 + (**ptr - '0');
        (*ptr)++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const char *s;
    for (size_t i = 0; i < row_count; ++i) {
        s = rows[i];
        // Parse u parameter, matches original p0 extraction
        s += 2; // Skip "u=" prefix
        int u = parse_decimal(&s);
        s += 1; // Skip '&' separator
        // Parse start parameter, matches original p1 extraction
        s += 6; // Skip "start=" prefix
        int s_val = parse_decimal(&s);
        s += 1; // Skip '&' separator
        // Parse finish parameter, matches original p2 extraction
        s += 5; // Skip "finish=" prefix
        int f = parse_decimal(&s);
        // Exact same calculation as baseline to preserve checksum correctness
        uint64_t v = (uint64_t)((u & 2047) + s_val + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}