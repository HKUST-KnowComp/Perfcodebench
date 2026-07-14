#include "interface.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];
        // Skip past unused id field to first comma
        while (*s != ',') s++;
        s++;
        // Skip past unused a field to second comma
        while (*s != ',') s++;
        s++;
        // Parse b field
        int b = 0;
        int neg = (*s == '-');
        if (neg) s++;
        while (*s >= '0' && *s <= '9') {
            b = b * 10 + (*s - '0');
            s++;
        }
        if (neg) b = -b;
        s++;
        // Parse c field
        int c = 0;
        neg = (*s == '-');
        if (neg) s++;
        while (*s >= '0' && *s <= '9') {
            c = c * 10 + (*s - '0');
            s++;
        }
        if (neg) c = -c;
        s++;
        // Check for "hot" flag without strcmp overhead
        int is_hot = (s[0] == 'h' && s[1] == 'o' && s[2] == 't' && (s[3] == '\0' || s[3] == ','));
        // Maintain exact same hash logic as original
        uint64_t v = (uint64_t)(b + c + 19);
        if (is_hot) v += 29ULL;
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}