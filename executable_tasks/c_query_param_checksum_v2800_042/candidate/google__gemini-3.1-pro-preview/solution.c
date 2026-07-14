#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized query parameter scanner.
 * 
 * The baseline implementation uses strcpy, strtok, and atoi.
 * This version parses the integers directly from the source strings,
 * avoiding memory copies and redundant string scanning.
 */

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        // Parse 'u' value starting at p + 2 (skipping "u=")
        p += 2;
        int u = 0;
        int sign_u = 1;
        if (*p == '-') { sign_u = -1; p++; } else if (*p == '+') { p++; }
        while (*p >= '0' && *p <= '9') {
            u = u * 10 + (*p - '0');
            p++;
        }
        u *= sign_u;

        // Move to the start of the second token (after '&')
        while (*p && *p != '&') p++;
        if (*p) p++;

        // Parse 's' value starting at p + 6 (skipping "status")
        // Note: baseline uses p1 + 6, which points to the '=' in "status=..."
        // atoi("=...") returns 0.
        p += 6;
        int s = 0;
        int sign_s = 1;
        if (*p == '-') { sign_s = -1; p++; } else if (*p == '+') { p++; }
        while (*p >= '0' && *p <= '9') {
            s = s * 10 + (*p - '0');
            p++;
        }
        s *= sign_s;

        // Move to the start of the third token (after '&')
        while (*p && *p != '&') p++;
        if (*p) p++;

        // Parse 'f' value starting at p + 5 (skipping "flag=")
        p += 5;
        int f = 0;
        int sign_f = 1;
        if (*p == '-') { sign_f = -1; p++; } else if (*p == '+') { p++; }
        while (*p >= '0' && *p <= '9') {
            f = f * 10 + (*p - '0');
            p++;
        }
        f *= sign_f;

        // Compute value and update FNV-1a hash
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }

    return hash;
}