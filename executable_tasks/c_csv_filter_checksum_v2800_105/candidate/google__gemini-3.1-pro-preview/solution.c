#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized CSV row filtering and checksum calculation.
 * Replaces sscanf with manual parsing to avoid overhead.
 */
uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        // Skip 'r' and the first two comma-separated fields (id and a)
        if (*p == 'r') p++;
        while (*p != ',' && *p != '\0') p++; // skip id
        if (*p == ',') p++;
        while (*p != ',' && *p != '\0') p++; // skip a
        if (*p == ',') p++;

        // Parse integer b
        int b = 0;
        int neg_b = 0;
        if (*p == '-') { neg_b = 1; p++; }
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            p++;
        }
        if (neg_b) b = -b;
        if (*p == ',') p++;

        // Parse integer c
        int c = 0;
        int neg_c = 0;
        if (*p == '-') { neg_c = 1; p++; }
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            p++;
        }
        if (neg_c) c = -c;
        if (*p == ',') p++;

        // Calculate base value v
        uint64_t v = (uint64_t)(b + c + 20);

        // Check for "hot" flag. sscanf %7s stops at whitespace or 7 chars.
        // We check if the next 3 chars are 'h', 'o', 't' and the 4th is a terminator.
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            char c4 = p[3];
            // sscanf %s stops at any whitespace (space, tab, newline, etc.) or null.
            if (c4 == '\0' || c4 == ' ' || c4 == '\t' || c4 == '\n' || c4 == '\r') {
                v += 29ULL;
            }
        }

        // FNV-1a style hash update
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }

    return hash;
}