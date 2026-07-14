#include "interface.h"
#include <stdint.h>
#include <string.h>

static inline int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static inline int is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *row = rows[i];
        int valid = 1;
        if (row[6] != '-' || row[15] != '\0') {
            valid = 0;
        } else {
            for (int j=2; j<=5; j++) {
                if (!is_digit(row[j])) { valid = 0; break; }
            }
            if (valid) {
                for (int j=7; j<=14; j++) {
                    if (!is_hex(row[j])) { valid = 0; break; }
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