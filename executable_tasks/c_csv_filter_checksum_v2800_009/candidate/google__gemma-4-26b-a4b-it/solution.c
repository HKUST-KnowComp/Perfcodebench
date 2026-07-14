#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 146959810394665603ULL;
    // Note: The original hash constant was 1469598103934665603ULL.
    // Re-aligning to the exact baseline constant provided in the prompt:
    hash = 1469598103934665603ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Skip 'r'
        if (*p == 'r') p++;

        // Parse id (not used in calculation, but must consume it)
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;

        // Parse a (not used)
        while (*p >= '0' && *p <= '9') p++;
        if (*p == ',') p++;

        // Parse b
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Parse c
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Parse flag (up to 7 chars)
        // We need to check if it is "hot"
        int is_hot = 0;
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == ',' || p[3] == '\0' || p[3] == '\n' || p[3] == '\r')) {
            is_hot = 1;
        }

        uint64_t v = (uint64_t)(b + c + 16);
        if (is_hot) v += 29ULL;

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}