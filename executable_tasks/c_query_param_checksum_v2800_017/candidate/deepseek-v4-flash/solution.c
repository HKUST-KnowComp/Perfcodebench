#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char *p) {
    int sign = 1;
    int val = 0;
    /* Skip possible leading whitespace (minimal, for safety) */
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '-') { sign = -1; ++p; }
    else if (*p == '+') { ++p; }
    if (*p < '0' || *p > '9') return 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        ++p;
    }
    return sign * val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *row = rows[i];
        /* First token starts at row[0] */
        const char *p0 = row;
        /* Find first '&' => end of first token, start of second token */
        const char *p1 = row;
        while (*p1 && *p1 != '&') ++p1;
        /* p1 now points to '&' or end; assume '&' exists */
        ++p1; /* now points to start of second token */
        /* Find second '&' for third token */
        const char *p2 = p1;
        while (*p2 && *p2 != '&') ++p2;
        ++p2; /* start of third token */

        int u = fast_atoi(p0 + 2);
        int s = fast_atoi(p1 + 6);
        int f = fast_atoi(p2 + 5);

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}