#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi(const char* s, const char* end) {
    int val = 0;
    while (s < end && *s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        int u = 0, s = 0, f = 0;

        // Manual scan to avoid strtok/strcpy overhead
        // Expected format: u=...&s=...&f=...
        // We look for the '=' and then parse the number until '&' or '\0'
        
        // Parse u
        while (*p && *p != 'u') p++;
        if (*p == 'u') {
            p += 2; // skip 'u='
            const char* start = p;
            while (*p && *p != '&') p++;
            u = fast_atoi(start, p);
        }

        // Parse s
        while (*p && *p != 's') p++;
        if (*p == 's') {
            p += 2; // skip 's='
            const char* start = p;
            while (*p && *p != '&') p++;
            s = fast_atoi(start, p);
        }

        // Parse f
        while (*p && *p != 'f') p++;
        if (*p == 'f') {
            p += 2; // skip 'f='
            const char* start = p;
            while (*p && *p != '&' && *p != '\0') p++;
            f = fast_atoi(start, p);
        }

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}