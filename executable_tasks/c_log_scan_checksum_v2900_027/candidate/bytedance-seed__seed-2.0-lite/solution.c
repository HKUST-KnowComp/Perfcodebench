#include "interface.h"
#include <stdint.h>

static inline int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        int valid = 0;
        if (row[15] == '\0' && row[6] == '-') {
            if (is_digit(row[2]) && is_digit(row[3]) && is_digit(row[4]) && is_digit(row[5])) {
                if (is_hex(row[7]) && is_hex(row[8]) && is_hex(row[9]) && is_hex(row[10]) &&
                    is_hex(row[11]) && is_hex(row[12]) && is_hex(row[13]) && is_hex(row[14])) {
                    valid = 1;
                }
            }
        }
        uint64_t v = 0;
        if (valid) {
            v = (uint64_t)((unsigned char)row[2] + (unsigned char)row[7]);
        }
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}