#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        uint64_t v = 1ULL;
        
        if (s[15] == '\0' && s[6] == '-' &&
            s[2] >= '0' && s[2] <= '9' && s[3] >= '0' && s[3] <= '9' &&
            s[4] >= '0' && s[4] <= '9' && s[5] >= '0' && s[5] <= '9') {
            
            int ok = 1;
            for (int j = 7; j < 15; ++j) {
                char c = s[j];
                if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                    ok = 0; break;
                }
            }
            if (ok) {
                v = (uint64_t)((unsigned char)s[2] + (unsigned char)s[7]) + 1ULL;
            }
        }
        
        hash ^= v;
        hash *= prime;
    }
    return hash;
}
