#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Fast integer parsing for specific offsets
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
        
        // The baseline logic assumes a specific structure:
        // p0: "u=..." (offset 2 is start of int)
        // p1: "s=..." (offset 6 is start of int, implying "key=val&" pattern)
        // p2: "f=..." (offset 5 is start of int)
        // However, strtok behavior and atoi offsets suggest the keys are fixed length.
        // To preserve exact correctness of the baseline's logic:
        
        // Find first '&'
        const char* p0_end = s;
        while (*p0_end != '&' && *p0_end != '\0') p0_end++;
        
        // Find second '&'
        const char* p1_end = p0_end + 1;
        while (*p1_end != '&' && *p1_end != '\0') p1_end++;
        
        // Find third '&'
        const char* p2_end = p1_end + 1;
        while (*p2_end != '&' && *p2_end != '\0') p2_end++;

        // Replicating the exact pointer arithmetic of the baseline:
        // p0 + 2, p1 + 6, p2 + 5
        // We must find the start of the segments to mimic strtok's behavior
        
        // Segment 0
        int u = fast_atoi(s + 2);
        
        // Segment 1 (starts after first '&')
        // The baseline used strtok(NULL, "&") which moves pointer to after the first '&'
        // Then it did p1 + 6. This implies the second segment is 6 chars before the value.
        // We find the start of the second segment by finding the first '&'
        const char* seg1 = s;
        while (*seg1 != '&' && *seg1 != '\0') seg1++;
        seg1++; // move past '&'
        int s_val = fast_atoi(seg1 + 6);

        // Segment 2 (starts after second '&')
        const char* seg2 = seg1;
        while (*seg2 != '&' && *seg2 != '\0') seg2++;
        seg2++; // move past '&'
        int f_val = fast_atoi(seg2 + 5);

        uint64_t v = (uint64_t)((u & 2047) + s_val + f_val * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}