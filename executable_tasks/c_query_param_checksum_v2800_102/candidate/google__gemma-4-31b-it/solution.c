#include "interface.h"

#include <stdint.h>
#include <stddef.h>

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
        const char* p = rows[i];
        
        // Skip "u="
        p += 2;
        int u = fast_atoi(p);
        
        // Find next '&' and skip "s="
        while (*p != '&') p++;
        p += 2; // skip '&s' (Wait, baseline says p1 + 6, which is "s=value")
        // Let's be precise: baseline p0+2, p1+6, p2+5
        // p0 is "u=...", p0+2 is the value
        // p1 is "s=...", p1+2 is the value. 
        // Wait, the baseline says p1 + 6. Let's re-examine: 
        // If row is "u=123&s=456&f=789", strtok(buf, "&") gives "u=123". p0+2 is "123".
        // strtok(NULL, "&") gives "s=456". p1+2 is "456".
        // The baseline says p1 + 6 and p2 + 5. This implies the keys are longer.
        // Example: "user=123&status=456&flag=789"
        // p0: "user=123" -> p0+5 is value. Baseline says p0+2. 
        // Let's look at the baseline again: p0+2, p1+6, p2+5.
        // This means keys are 2, 6, and 5 chars long respectively (including '=').
        // Key 1: 2 chars (e.g., "u=")
        // Key 2: 6 chars (e.g., "status=")
        // Key 3: 5 chars (e.g., "flag=")
        
        // Correcting the manual scan to match baseline offsets:
        const char* curr = rows[i];
        
        // Field 1: offset 2
        int val_u = fast_atoi(curr + 2);
        
        // Field 2: find '&', then offset 6 from start of that token
        while (*curr != '&') curr++;
        curr++; // move past '&'
        int val_s = fast_atoi(curr + 6);
        
        // Field 3: find '&', then offset 5 from start of that token
        while (*curr != '&') curr++;
        curr++; // move past '&'
        int val_f = fast_atoi(curr + 5);
        
        uint64_t v = (uint64_t)((val_u & 2047) + val_s + val_f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}