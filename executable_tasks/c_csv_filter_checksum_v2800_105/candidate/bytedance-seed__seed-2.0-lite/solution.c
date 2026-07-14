#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        int commas = 0;
        // Skip unused leading fields to reach the b value (third CSV field)
        while (*p && commas < 2) {
            if (*p == ',') commas++;
            p++;
        }
        // Parse integer b
        int b = 0;
        while (*p != ',') {
            b = b * 10 + (*p - '0');
            p++;
        }
        p++; // Move past comma to start of c
        // Parse integer c
        int c = 0;
        while (*p != ',') {
            c = c * 10 + (*p - '0');
            p++;
        }
        p++; // Move past comma to start of flag field
        // Calculate value matching original logic, fast flag check
        uint64_t v = (uint64_t)(b + c + 20);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            v += 29ULL;
        }
        // Update hash identically to original implementation
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}