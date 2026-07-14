#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        // Skip leading 'r' and unused id field
        p++;
        while (*p != ',') p++;
        p++;
        // Skip unused a field
        while (*p != ',') p++;
        p++;
        // Parse required b value
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        p++;
        // Parse required c value
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        p++;
        // Exact "hot" flag check matching original strcmp behavior, no library call
        int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0');
        // Match original value calculation exactly
        uint64_t v = (uint64_t)(b + c + 14);
        if (is_hot) v += 29ULL;
        // Preserve original hash computation
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}