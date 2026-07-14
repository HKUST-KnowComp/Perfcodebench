#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        uint64_t v = 1ULL;

        if (r[6] == '-' && r[15] == '\0' &&
            r[2] >= '0' && r[2] <= '9' && r[3] >= '0' && r[3] <= '9' &&
            r[4] >= '0' && r[4] <= '9' && r[5] >= '0' && r[5] <= '9') {
            int hex_ok = 1;
            for (int j = 7; j <= 14; ++j) {
                char c = r[j];
                if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                    hex_ok = 0;
                    break;
                }
            }
            if (hex_ok) {
                v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7] + 1ULL);
            }
        }

        hash ^= v;
        hash *= prime;
    }
    return hash;
}
