#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized query parameter scanner.
 * 
 * The baseline used strcpy, strtok, and atoi, which are slow due to memory copying,
 * multiple passes over the string, and general-purpose overhead.
 * 
 * This implementation performs a single pass to extract 'u', 'score', and 'flag'
 * values directly from the input strings and computes the FNV-1a-style hash.
 */
uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];

        // Parse 'u' value (starts at p + 2, after "u=")
        p += 2;
        int u = 0;
        while (*p >= '0' && *p <= '9') {
            u = u * 10 + (*p - '0');
            p++;
        }

        // Skip '&score=' (p is currently at '&')
        // "&score=" is 7 characters
        p += 7;
        int s = 0;
        while (*p >= '0' && *p <= '9') {
            s = s * 10 + (*p - '0');
            p++;
        }

        // Skip '&flag=' (p is currently at '&')
        // "&flag=" is 6 characters
        p += 6;
        int f = 0;
        while (*p >= '0' && *p <= '9') {
            f = f * 10 + (*p - '0');
            p++;
        }

        // Compute row value and update hash
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }

    return hash;
}