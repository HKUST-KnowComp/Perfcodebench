#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* q = rows[i];
        int u = 0;
        q += 2;
        while (*q != '&') {
            u = u * 10 + (*q - '0');
            q++;
        }
        q++;
        int s = 0;
        q += 6;
        while (*q != '&') {
            s = s * 10 + (*q - '0');
            q++;
        }
        q++;
        int f = 0;
        q += 5;
        while (*q != '\0') {
            f = f * 10 + (*q - '0');
            q++;
        }
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}