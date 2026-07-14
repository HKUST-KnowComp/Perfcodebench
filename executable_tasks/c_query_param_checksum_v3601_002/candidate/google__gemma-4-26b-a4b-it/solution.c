#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* p, const char* end) {
    int val = 0;
    while (p < end && *p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        int u = 0, s = 0, f = 0;

        // Manual scan to find u, s, f without strtok or strcpy
        // Expected format: u=X&s=Y&f=Z
        while (*p != '\0') {
            if (p[0] == 'u' && p[1] == '=') {
                p += 2;
                const char* start = p;
                while (*p >= '0' && *p <= '9') p++;
                u = fast_atoi(start, p);
            } else if (p[0] == 's' && p[1] == '=') {
                p += 2;
                const char* start = p;
                while (*p >= '0' && *p <= '9') p++;
                s = fast_atoi(start, p);
            } else if (p[0] == 'f' && p[1] == '=') {
                p += 2;
                const char* start = p;
                while (*p >= '0' && *p <= '9') p++;
                f = fast_atoi(start, p);
            } else {
                p++;
            }
            
            // Skip to next delimiter if we found a value
            if (*p == '&') p++;
        }

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}