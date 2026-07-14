#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Optimized manual parser to replace sscanf
static inline uint64_t process_row(const char* s) {
    // Format: r%d,%d,%d,%d,%7s
    // We need b, c, and the flag.
    
    // Skip 'r'
    s++;
    
    // Parse id (not used in calculation, but must skip)
    while (*s >= '0' && *s <= '9') s++;
    if (*s == ',') s++;

    // Parse a (not used)
    while (*s >= '0' && *s <= '9') s++;
    if (*s == ',') s++;

    // Parse b
    int b = 0;
    while (*s >= '0' && *s <= '9') {
        b = b * 10 + (*s - '0');
        s++;
    }
    if (*s == ',') s++;

    // Parse c
    int c = 0;
    while (*s >= '0' && *s <= '9') {
        c = c * 10 + (*s - '0');
        s++;
    }
    if (*s == ',') s++;

    // Parse flag (check if it starts with 'hot')
    // The flag is the last field. We check if it matches 'hot'
    int is_hot = 0;
    if (s[0] == 'h' && s[1] == 'o' && s[2] == 't' && (s[3] == ',' || s[3] == '\0' || s[3] == '\n' || s[3] == '\r')) {
        is_hot = 1;
    }

    uint64_t v = (uint64_t)(b + c + 13);
    if (is_hot) v += 29ULL;
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        uint64_t v = process_row(rows[i]);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}