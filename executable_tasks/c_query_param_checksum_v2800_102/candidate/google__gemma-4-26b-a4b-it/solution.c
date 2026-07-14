#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline int fast_atoi_offset(const char* p, int offset) {
    int val = 0;
    p += offset;
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
        
        // Find p0 (u) - assuming format u=X&...
        // The original code used atoi(p0 + 2), implying 'u=' is 2 chars
        const char* p0 = s;
        const char* amp1 = NULL;
        for (const char* c = s; *c != '\0'; ++c) {
            if (*c == '&') {
                amp1 = c;
                break;
            }
        }

        // Find p1 (s) - assuming format s=XXXXXX&...
        // The original code used atoi(p1 + 6), implying 's=' is 2 chars + 4 offset?
        // Wait, the original code: p1 = strtok(NULL, "&"); s = atoi(p1 + 6);
        // This implies the second param is something like "key=val" where key is 4 chars?
        // Let's look at the offsets: p0+2, p1+6, p2+5.
        // This suggests specific key lengths: p0 is 2 chars (e.g. "u="), p1 is 6 chars (e.g. "param="), p2 is 5 chars (e.g. "f==").
        // To be safe and fast, we find the '&' delimiters and apply the exact offsets.
        
        const char* p1_start = amp1 ? amp1 + 1 : NULL;
        const char* amp2 = NULL;
        if (p1_start) {
            for (const char* c = p1_start; *c != '\0'; ++c) {
                if (*c == '&') {
                    amp2 = c;
                    break;
                }
            }
        }

        const char* p2_start = amp2 ? amp2 + 1 : NULL;

        // Replicating the exact logic of the original atoi(p + offset)
        // We must handle the case where strtok might return NULL, though the task implies valid rows.
        int u = 0, s = 0, f = 0;
        
        // p0
        u = fast_atoi_offset(p0, 2);
        
        // p1
        if (p1_start) {
            s = fast_atoi_offset(p1_start, 6);
        }
        
        // p2
        if (p2_start) {
            f = fast_atoi_offset(p2_start, 5);
        }

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}