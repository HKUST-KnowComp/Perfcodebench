#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized CSV row filtering and checksum calculation.
 * Replaces sscanf with a manual parser to avoid format string overhead and unnecessary field extraction.
 */
uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        // Skip 'r' and the 'id' field
        if (*p == 'r') p++;
        while (*p != ',' && *p != '\0') p++;
        if (*p == ',') p++;

        // Skip the 'a' field
        while (*p != ',' && *p != '\0') p++;
        if (*p == ',') p++;

        // Parse the 'b' field
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

        // Parse the 'c' field
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
        uint64_t v = (uint64_t)(b + c + 7);

        // Check for "hot" flag efficiently
        // sscanf %7s stops at whitespace or max length, but CSV usually uses commas.
        // The baseline uses strcmp(flag, "hot"), so we check for exact match.
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == '\0' || p[3] == ' ' || p[3] == '\t' || p[3] == '\n' || p[3] == '\r')) {
            v += 29ULL;
        }

        // FNV-1a style hash update
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }

    return hash;
}