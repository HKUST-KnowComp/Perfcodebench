#include "interface.h"
#include <stdint.h>
#include <stddef.h>

// Optimized single-pass parser to avoid strcpy, strtok, and atoi overhead.
// The original logic expects: 
// p0: 'u=...' (offset 2)
// p1: 's=...' (offset 6 - wait, original code used p1+6, implying 's=...' is not the key, but rather a specific offset in the second token)
// p2: 'f=...' (offset 5)
// Let's re-examine the original logic: 
// p0 = strtok(buf, "&") -> e.g., "u=123"
// p1 = strtok(NULL, "&") -> e.g., "s=456"
// p2 = strtok(NULL, "&") -> e.g., "f=789"
// u = atoi(p0 + 2) -> skips "u="
// s = atoi(p1 + 6) -> skips 6 chars (e.g., "param_s=123" -> "123")
// f = atoi(p2 + 5) -> skips 5 chars (e.g., "field_f=123" -> "123")
// Since the exact key names aren't provided, we must replicate the exact pointer offsets used in the baseline.

static inline int fast_atoi(const char* s) {
    int res = 0;
    while (*s >= '0' && *s <= '9') {
        res = res * 10 + (*s - '0');
        s++;
    }
    return res;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        
        // Find first '&'
        const char* p0 = p;
        while (*p0 != '&' && *p0 != '\0') p0++;
        
        // Find second '&'
        const char* p1 = p0 + 1;
        while (*p1 != '&' && *p1 != '\0') p1++;
        
        // Find third '&'
        const char* p2 = p1 + 1;
        while (*p2 != '&' && *p2 != '\0') p2++;

        // Replicate the exact offset logic from baseline:
        // u = atoi(p0 + 2)
        // s = atoi(p1 + 6)
        // f = atoi(p2 + 5)
        int u = fast_atoi(p0 + 2);
        int s = fast_atoi(p1 + 6);
        int f = fast_atoi(p2 + 5);

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}