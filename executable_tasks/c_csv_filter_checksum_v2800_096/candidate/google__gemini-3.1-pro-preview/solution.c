#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized CSV row filtering and checksum calculation.
 * Replaces sscanf with manual parsing and strcmp with direct character comparison.
 */
uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        // Skip 'r'
        if (*p == 'r') p++;

        // Skip 'id' field
        while (*p != ',' && *p != '\0') p++;
        if (*p == ',') p++;

        // Skip 'a' field
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
        uint64_t v = (uint64_t)(b + c + 11);

        // Check if flag is "hot"
        // sscanf %s stops at whitespace, but in CSV context we check for 'hot' specifically.
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ' ' || p[3] == '\t' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }

        // FNV-1a style hash update
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }

    return hash;
}
