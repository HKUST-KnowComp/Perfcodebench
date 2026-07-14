#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // parse u (skip "u=")
        const char* dig = p + 2;
        int u = 0;
        while (*dig >= '0' && *dig <= '9') {
            u = u * 10 + (*dig - '0');
            ++dig;
        }
        if (*dig == '&') ++dig;
        p = dig;
        
        // parse s (skip "status=")
        dig = p + 6;
        int s = 0;
        while (*dig >= '0' && *dig <= '9') {
            s = s * 10 + (*dig - '0');
            ++dig;
        }
        if (*dig == '&') ++dig;
        p = dig;
        
        // parse f (skip "flags=")
        dig = p + 5;
        int f = 0;
        while (*dig >= '0' && *dig <= '9') {
            f = f * 10 + (*dig - '0');
            ++dig;
        }
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
