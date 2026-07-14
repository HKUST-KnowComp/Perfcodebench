#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline uint64_t fast_atoi(const char** s) {
    uint64_t val = 0;
    while (**s >= '0' && **s <= '9') {
        val = val * 10 + (**s - '0');
        (*s)++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'r'
        if (*p == 'r') p++;
        
        // Parse id
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;

        // Parse a
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;

        // Parse b
        uint64_t b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Parse c
        uint64_t c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Parse flag
        // We need to check if flag is "hot"
        // The flag starts after the 4th comma
        int is_hot = 0;
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == ',' || p[3] == '\0' || p[3] == '\n' || p[3] == '\r')) {
            is_hot = 1;
        }

        uint64_t v = b + c + 11;
        if (is_hot) v += 29ULL;

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}