#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 0;
        unsigned char c2 = (unsigned char)s[2];
        unsigned char c7 = (unsigned char)s[7];
        if (s[6] == '-' && s[15] == '\0' &&
            c2 >= '0' && c2 <= '9' &&
            s[3] >= '0' && s[3] <= '9' &&
            s[4] >= '0' && s[4] <= '9' &&
            s[5] >= '0' && s[5] <= '9' &&
            ((c7 >= '0' && c7 <= '9') || (c7 >= 'A' && c7 <= 'F') || (c7 >= 'a' && c7 <= 'f'))) {
            v = (uint64_t)(c2 + c7);
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}