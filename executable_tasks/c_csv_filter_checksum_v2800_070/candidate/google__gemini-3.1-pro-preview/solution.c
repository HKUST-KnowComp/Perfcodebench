#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized CSV row filtering and checksum calculation.
 * Replaces sscanf with a manual parser to avoid overhead.
 * Skips unused fields 'id' and 'a', parses 'b' and 'c' directly,
 * and performs a fast check for the 'hot' flag.
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
        int b_sign = 1;
        if (*p == '-') {
            b_sign = -1;
            p++;
        }
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        b *= b_sign;
        if (*p == ',') p++;

        // Parse the fourth field (c)
        int c = 0;
        int c_sign = 1;
        if (*p == '-') {
            c_sign = -1;
            p++;
        }
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        c *= c_sign;
        if (*p == ',') p++;

        // Calculate base value v
        uint64_t v = (uint64_t)(b + c + 8);

        // Check for the "hot" flag in the fifth field
        // sscanf %7s stops at whitespace or end of string.
        // We check for 'h', 'o', 't' followed by a terminator.
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ' ' || p[3] == '\t' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }

        // FNV-1a style hash update
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }

    return hash;
}