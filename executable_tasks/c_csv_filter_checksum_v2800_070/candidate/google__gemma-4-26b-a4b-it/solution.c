#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'r' and parse id
        if (*p == 'r') p++;
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;

        // Parse a
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;

        // Parse b
        int64_t b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Parse c
        int64_t c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Parse d (not used in checksum, but must skip to reach flag)
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;

        // Check flag (must be "hot")
        // We check if the next characters are 'h', 'o', 't' and then a comma or null
        int is_hot = 0;
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            if (p[3] == ',' || p[3] == '\0') {
                is_hot = 1;
            }
        }

        uint64_t v = (uint64_t)(b + c + 8);
        if (is_hot) v += 29ULL;
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}