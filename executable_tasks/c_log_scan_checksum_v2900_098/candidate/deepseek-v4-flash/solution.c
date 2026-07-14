#include "interface.h"
#include <stdint.h>
#include <string.h>

static inline int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *r = rows[i];
        uint64_t v = 0;

        if (strlen(r) == 15 &&
            is_digit(r[2]) && is_digit(r[3]) && is_digit(r[4]) && is_digit(r[5]) &&
            r[6] == '-' &&
            is_hex(r[7]) && is_hex(r[8]) && is_hex(r[9]) && is_hex(r[10]) &&
            is_hex(r[11]) && is_hex(r[12]) && is_hex(r[13]) && is_hex(r[14])) {
            v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
