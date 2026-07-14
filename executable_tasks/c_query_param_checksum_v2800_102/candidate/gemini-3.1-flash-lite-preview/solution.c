#include "interface.h"
#include <stdint.h>

static inline int fast_atoi(const char* p) {
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        
        // Format: "id=...&sid=...&fid=..."
        // Skip "id=" (3 chars)
        int u = fast_atoi(row + 3);
        
        // Find "sid=" (skip until '&')
        while (*row != '&') row++;
        row++; // skip '&'
        // Skip "sid=" (4 chars)
        int s = fast_atoi(row + 4);
        
        // Find "fid=" (skip until '&')
        while (*row != '&') row++;
        row++; // skip '&'
        // Skip "fid=" (4 chars)
        int f = fast_atoi(row + 4);
        
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}