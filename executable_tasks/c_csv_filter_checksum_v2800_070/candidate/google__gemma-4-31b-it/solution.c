#include "interface.h"

#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'r' and the first integer (id)
        p++; // skip 'r'
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip first comma

        // Parse 'a'
        while (*p >= '0' && *p <= '9') p++;
        p++; // skip second comma

        // Parse 'b'
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        p++; // skip third comma

        // Parse 'c'
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        p++; // skip fourth comma

        uint64_t v = (uint64_t)(b + c + 8);

        // Check if flag is "hot"
        // The flag is the last field. We check for 'h', 'o', 't' sequence.
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}