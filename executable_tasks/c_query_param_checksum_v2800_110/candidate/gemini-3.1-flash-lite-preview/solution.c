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
        const char* s = rows[i];
        
        // Format: "id=...&sid=...&fid=..."
        // Skip "id="
        int u = fast_atoi(s + 3);
        
        // Find "&sid="
        while (*s != '&') s++;
        s += 5; // skip "&sid="
        int sid = fast_atoi(s);
        
        // Find "&fid="
        while (*s != '&') s++;
        s += 5; // skip "&fid="
        int fid = fast_atoi(s);
        
        uint64_t v = (uint64_t)((u & 2047) + sid + fid * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}