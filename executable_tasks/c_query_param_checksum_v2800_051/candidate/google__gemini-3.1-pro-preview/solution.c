#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized query parameter scanner.
 * Replaces strcpy, strtok, and atoi with a single-pass manual scan.
 * Preserves the exact logic of the baseline: 
 * - u starts at p0 + 2
 * - s starts at p1 + 6 (which points to the '=' in "status=")
 * - f starts at p2 + 5 (which points to the first digit after "flag=")
 */
uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Parse 'u' field (starts at p + 2)
        p += 2;
        int u = 0;
        while (*p >= '0' && *p <= '9') {
            u = u * 10 + (*p - '0');
            p++;
        }
        
        // Skip to the start of the second field (after first '&')
        while (*p && *p != '&') p++;
        if (*p == '&') p++;
        
        // Parse 's' field (baseline uses p1 + 6)
        // p1 is the start of "status=...". p1 + 6 is the '=' character.
        // atoi("=...") returns 0.
        p += 6;
        int s = 0;
        if (*p >= '0' && *p <= '9') {
            while (*p >= '0' && *p <= '9') {
                s = s * 10 + (*p - '0');
                p++;
            }
        } else {
            // Baseline atoi("=") returns 0, so we skip to next '&'
            while (*p && *p != '&') p++;
        }

        // Skip to the start of the third field (after second '&')
        while (*p && *p != '&') p++;
        if (*p == '&') p++;

        // Parse 'f' field (baseline uses p2 + 5)
        // p2 is the start of "flag=...". p2 + 5 is the first digit.
        p += 5;
        int f = 0;
        while (*p >= '0' && *p <= '9') {
            f = f * 10 + (*p - '0');
            p++;
        }

        // Compute hash update using baseline formula
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }

    return hash;
}