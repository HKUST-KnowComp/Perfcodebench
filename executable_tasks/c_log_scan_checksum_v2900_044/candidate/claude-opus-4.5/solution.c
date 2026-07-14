#include "interface.h"

#include <stdint.h>

static inline int is_hex(unsigned char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char* r = (const unsigned char*)rows[i];
        uint64_t v = 0;
        /* Format: %c%c%4u-%8X means:
           pos 0-1: any char (matched by %c%c)
           pos 2-5: 4 decimal digits (matched by %4u)
           pos 6: dash '-'
           pos 7-14: 8 hex digits (matched by %8X)
           total length must be exactly 15 */
        if (r[0] != '\0' && r[1] != '\0' &&
            r[2] >= '0' && r[2] <= '9' &&
            r[3] >= '0' && r[3] <= '9' &&
            r[4] >= '0' && r[4] <= '9' &&
            r[5] >= '0' && r[5] <= '9' &&
            r[6] == '-' &&
            is_hex(r[7]) && is_hex(r[8]) && is_hex(r[9]) && is_hex(r[10]) &&
            is_hex(r[11]) && is_hex(r[12]) && is_hex(r[13]) && is_hex(r[14]) &&
            r[15] == '\0') {
            v = (uint64_t)(r[2] + r[7]);
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
