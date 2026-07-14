#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint64_t parse_uint64(const char* start, const char* end) {
    uint64_t val = 0;
    while (start < end && *start >= '0' && *start <= '9') {
        val = val * 10 + (uint64_t)(*start - '0');
        ++start;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t mult = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        // Find first '&'
        const char* p1 = row;
        while (*p1 && *p1 != '&') ++p1;
        // Extract first token: [row, p1), value starts at row+2
        const char* u_start = row + 2;
        uint64_t u = parse_uint64(u_start, p1);
        // Find second '&'
        const char* p2 = p1;
        if (*p2 == '&') ++p2;
        const char* s_start = p2;
        while (*p2 && *p2 != '&') ++p2;
        // Extract second token: [s_start, p2), value starts at s_start+6
        uint64_t s = parse_uint64(s_start + 6, p2);
        // Third token: from after second '&' to end
        const char* f_start = p2;
        if (*f_start == '&') ++f_start;
        const char* f_end = f_start;
        while (*f_end) ++f_end;
        // Extract third token: [f_start, f_end), value starts at f_start+5
        uint64_t f = parse_uint64(f_start + 5, f_end);
        uint64_t v = (u & 2047ULL) + s + f * 19ULL;
        hash ^= (v + 1ULL);
        hash *= mult;
    }
    return hash;
}
