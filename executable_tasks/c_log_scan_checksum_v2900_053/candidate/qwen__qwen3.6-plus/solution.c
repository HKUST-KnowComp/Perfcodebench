#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* r = rows[i];
        if (r[15] != '\0') continue;
        if (r[6] != '-') continue;
        
        if (r[2] < '0' || r[2] > '9') continue;
        if (r[3] < '0' || r[3] > '9') continue;
        if (r[4] < '0' || r[4] > '9') continue;
        if (r[5] < '0' || r[5] > '9') continue;
        
        int valid = 1;
        for (int j = 7; j < 15; ++j) {
            char c = r[j];
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
                valid = 0;
                break;
            }
        }
        if (!valid) continue;
        
        uint64_t v = (uint64_t)((unsigned char)r[2] + (unsigned char)r[7]);
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}