#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized CSV row filtering and checksum calculation.
 * Replaces sscanf with a manual pointer-based parser to minimize overhead.
 */
uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        // Skip 'r'
        if (*p == 'r') p++;

        // Skip 'id' field (until first comma)
        while (*p != ',' && *p != '\0') p++;
        if (*p == ',') p++;

        // Skip 'a' field (until second comma)
        while (*p != ',' && *p != '\0') p++;
        if (*p == ',') p++;

        // Parse 'b' field
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Parse 'c' field
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Calculate base value v
        uint64_t v = (uint64_t)(b + c + 13);

        // Check for 'hot' flag
        // sscanf %s skips leading whitespace and reads until next whitespace.
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v') p++;
        
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            // Ensure it's exactly "hot" (followed by whitespace or null)
            char next = p[3];
            if (next == '\0' || next == ' ' || next == '\t' || next == '\n' || next == '\r') {
                v += 29ULL;
            }
        }

        // Update FNV-1a style hash
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }

    return hash;
}
