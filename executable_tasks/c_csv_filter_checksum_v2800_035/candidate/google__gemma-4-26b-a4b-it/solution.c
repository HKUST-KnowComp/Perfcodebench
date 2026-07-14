#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Optimized manual parser to replace sscanf
static inline uint64_t process_row(const char* s) {
    int id = 0, a = 0, b = 0, c = 0;
    
    // Skip 'r'
    if (*s == 'r') s++;

    // Parse id
    while (*s >= '0' && *s <= '9') {
        id = id * 10 + (*s - '0');
        s++;
    }
    s++; // skip ','

    // Parse a
    while (*s >= '0' && *s <= '9') {
        a = a * 10 + (*s - '0');
        s++;
    }
    s++; // skip ','

    // Parse b
    while (*s >= '0' && *s <= '9') {
        b = b * 10 + (*s - '0');
        s++;
    }
    s++; // skip ','

    // Parse c
    while (*s >= '0' && *s <= '9') {
        c = c * 10 + (*s - '0');
        s++;
    }
    s++; // skip ','

    // Check for "hot"
    // The flag starts at current s. We check if it matches 'h', 'o', 't', and then ',' or '\0'
    int is_hot = 0;
    if (s[0] == 'h' && s[1] == 'o' && s[2] == 't') {
        // Ensure it's exactly "hot" by checking the next char
        if (s[3] == ',' || s[3] == '\0' || s[3] == '\n' || s[3] == '\r') {
            is_hot = 1;
        }
    }

    uint64_t v = (uint64_t)(b + c + 19);
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