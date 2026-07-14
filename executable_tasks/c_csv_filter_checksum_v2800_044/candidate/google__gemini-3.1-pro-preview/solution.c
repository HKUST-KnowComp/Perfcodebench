#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized CSV row filtering and checksum calculation.
 * Replaces sscanf with manual parsing to avoid overhead.
 * Skips unused fields 'id' and 'a' and directly parses 'b', 'c', and 'flag'.
 */
uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        // Skip 'r' and the first field (id)
        if (*p == 'r') p++;
        while (*p != ',' && *p != '\0') p++;
        if (*p == ',') p++;

        // Skip the second field (a)
        while (*p != ',' && *p != '\0') p++;
        if (*p == ',') p++;

        // Parse the third field (b)
        int b = 0;
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Parse the fourth field (c)
        int c = 0;
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (*p == ',') p++;

        // Calculate base value v + 1
        // Original: v = b + c + 28; hash ^= (v + 1)
        // Simplified: hash ^= (b + c + 29)
        uint64_t v_plus_1 = (uint64_t)(b + c + 29);

        // Check for "hot" flag
        // We check for 'h', 'o', 't' and ensure it's the end of the string or field
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ',')) {
            v_plus_1 += 29ULL;
        }

        // Update FNV-1a style hash
        hash ^= v_plus_1;
        hash *= fnv_prime;
    }

    return hash;
}