#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;

        // Check length 15 first (fastest rejection)
        // We check if s[14] is the last char and s[15] is null
        if (s[14] != '\0' || s[15] != '\0') goto next;

        // Format: %c%c%4u-%8X
        // Indices: 0 1 2345 6 78901234
        //         p0 p1  x   -   y

        // Validate x (indices 2, 3, 4, 5) - must be 4 digits
        if (!is_digit(s[2]) || !is_digit(s[3]) || !is_digit(s[4]) || !is_digit(s[5])) goto next;

        // Validate separator (index 6)
        if (s[6] != '-') goto next;

        // Validate y (indices 7 to 14) - must be 8 hex digits
        for (int j = 7; j < 15; ++j) {
            if (!is_hex(s[j])) goto next;
        }

        // If we reached here, it matches the sscanf pattern and length
        v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);

    next:
        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}