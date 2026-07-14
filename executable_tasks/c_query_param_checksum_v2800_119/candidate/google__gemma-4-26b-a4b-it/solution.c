#include "interface.h"

#include <stdint.h>
#include <stddef.h>

// Helper to parse an integer from a specific offset in a substring
// This avoids the overhead of strtok and atoi
static inline int fast_atoi_at_offset(const char* start, size_t offset) {
    const char* p = start + offset;
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
        
        // The original logic used strtok(buf, "&") which splits by '&'
        // p0: first segment, p1: second, p2: third
        // We need to find the positions of '&' to locate the segments
        
        const char* p0_start = row;
        const char* p0_end = NULL;
        const char* p1_start = NULL;
        const char* p1_end = NULL;
        const char* p2_start = NULL;

        // Find first '&'
        for (const char* p = row; *p != '\0'; ++p) {
            if (*p == '&') {
                p0_end = p;
                p1_start = p + 1;
                break;
            }
        }

        if (p1_start) {
            // Find second '&'
            for (const char* p = p1_start; *p != '\0'; ++p) {
                if (*p == '&') {
                    p1_end = p;
                    p2_start = p + 1;
                    break;
                }
            }
        }

        // If we found all three segments, perform the math
        // Note: The original code assumes segments exist and follow specific patterns
        // p0 + 2, p1 + 6, p2 + 5
        if (p2_start) {
            int u = fast_atoi_at_offset(p0_start, 2);
            int s = fast_atoi_at_offset(p1_start, 6);
            int f = fast_atoi_at_offset(p2_start, 5);
            
            uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
            hash ^= (v + 1ULL);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
