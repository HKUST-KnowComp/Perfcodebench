#include "interface.h"

#include <stdint.h>

static inline int is_digit(unsigned char c) {
    return (unsigned)(c - '0') <= 9u;
}

static inline int is_hex(unsigned char c) {
    return (unsigned)(c - '0') <= 9u || (unsigned)((c | 0x20) - 'a') <= 5u;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char* r = (const unsigned char*)rows[i];
        uint64_t v = 0;
        /* Check length == 15 and format: ??DDDD-HHHHHHHH */
        if (r[15] == '\0' &&
            is_digit(r[2]) && is_digit(r[3]) && is_digit(r[4]) && is_digit(r[5]) &&
            r[7] == '-' &&
            is_hex(r[8]) && is_hex(r[9]) && is_hex(r[10]) && is_hex(r[11]) &&
            is_hex(r[12]) && is_hex(r[13]) && is_hex(r[14]) && is_hex(r[6])) {
            v = (uint64_t)(r[2] + r[7]);
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
