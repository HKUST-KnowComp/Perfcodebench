#include "interface.h"
#include <stdint.h>
#include <string.h>

static inline int is_hex(unsigned char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;
        if (strlen(s) == 15) {
            unsigned char c2 = (unsigned char)s[2];
            if (c2 >= '0' && c2 <= '9' &&
                (unsigned char)s[3] >= '0' && (unsigned char)s[3] <= '9' &&
                (unsigned char)s[4] >= '0' && (unsigned char)s[4] <= '9' &&
                (unsigned char)s[5] >= '0' && (unsigned char)s[5] <= '9' &&
                s[6] == '-' &&
                is_hex((unsigned char)s[7]) && is_hex((unsigned char)s[8]) &&
                is_hex((unsigned char)s[9]) && is_hex((unsigned char)s[10]) &&
                is_hex((unsigned char)s[11]) && is_hex((unsigned char)s[12]) &&
                is_hex((unsigned char)s[13]) && is_hex((unsigned char)s[14])) {
                v = (uint64_t)(c2 + (unsigned char)s[7]);
            }
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
