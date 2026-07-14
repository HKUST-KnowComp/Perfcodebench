#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline int is_digit(char c) { return c >= '0' && c <= '9'; }
static inline int is_hex(char c) {
    return is_digit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;
        
        // Fast equivalent to: sscanf(...) == 4 && strlen(s) == 15
        if (s[15] == '\0' && s[6] == '-' &&
            is_digit(s[2]) && is_digit(s[3]) && is_digit(s[4]) && is_digit(s[5]) &&
            is_hex(s[7]) && is_hex(s[8]) && is_hex(s[9]) && is_hex(s[10]) &&
            is_hex(s[11]) && is_hex(s[12]) && is_hex(s[13]) && is_hex(s[14])) {
            v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]);
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}