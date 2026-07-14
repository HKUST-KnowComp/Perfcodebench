#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Parse u: skip "u=" (2 chars)
        p += 2;
        int u = 0;
        while (*p >= '0' && *p <= '9') {
            u = u * 10 + (*p - '0');
            p++;
        }
        
        // Parse s: skip "&<5chars>=" (7 chars)
        p += 7;
        int s = 0;
        while (*p >= '0' && *p <= '9') {
            s = s * 10 + (*p - '0');
            p++;
        }
        
        // Parse f: skip "&<4chars>=" (6 chars)
        p += 6;
        int f = 0;
        while (*p >= '0' && *p <= '9') {
            f = f * 10 + (*p - '0');
            p++;
        }
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}