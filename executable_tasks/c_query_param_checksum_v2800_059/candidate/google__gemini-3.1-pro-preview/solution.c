#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized query parameter scanner.
 * Replaces strcpy/strtok/atoi with manual pointer arithmetic and fast integer parsing.
 * Preserves the exact logic of the baseline, including the specific offsets used.
 */

static inline int fast_atoi(const char* s) {
    int val = 0;
    // Handle optional sign as atoi does
    if (*s == '+') s++;
    else if (*s == '-') {
        s++;
        while (*s >= '0' && *s <= '9') {
            val = val * 10 + (*s - '0');
            s++;
        }
        return -val;
    }
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        
        // The baseline uses strtok which splits by '&'.
        // p0 = strtok(buf, "&") -> starts at row[0]
        // p1 = strtok(NULL, "&") -> starts after first '&'
        // p2 = strtok(NULL, "&") -> starts after second '&'
        
        // Extract u from p0 + 2
        int u = fast_atoi(row + 2);

        // Find first '&' to locate p1
        const char* p1 = row;
        while (*p1 && *p1 != '&') p1++;
        if (*p1 == '&') p1++;

        // Extract s from p1 + 6
        int s = fast_atoi(p1 + 6);

        // Find second '&' to locate p2
        const char* p2 = p1;
        while (*p2 && *p2 != '&') p2++;
        if (*p2 == '&') p2++;

        // Extract f from p2 + 5
        int f = fast_atoi(p2 + 5);

        // Compute value and update hash
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    
    return hash;
}