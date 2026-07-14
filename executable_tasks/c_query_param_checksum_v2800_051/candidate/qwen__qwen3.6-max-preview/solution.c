#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const char* const* end = rows + row_count;
    for (; rows < end; ++rows) {
        const char* p = *rows;
        
        // Parse u
        while (*p != '=') p++;
        p++;
        int u = 0;
        if (*p == '-') {
            p++;
            while ((unsigned char)(*p - '0') <= 9) { u = u * 10 - (*p - '0'); p++; }
        } else {
            while ((unsigned char)(*p - '0') <= 9) { u = u * 10 + (*p - '0'); p++; }
        }

        // Skip to s
        while (*p != '&') p++;
        p++;
        while (*p != '=') p++;
        p++;
        int s = 0;
        if (*p == '-') {
            p++;
            while ((unsigned char)(*p - '0') <= 9) { s = s * 10 - (*p - '0'); p++; }
        } else {
            while ((unsigned char)(*p - '0') <= 9) { s = s * 10 + (*p - '0'); p++; }
        }

        // Skip to f
        while (*p != '&') p++;
        p++;
        while (*p != '=') p++;
        p++;
        int f = 0;
        if (*p == '-') {
            p++;
            while ((unsigned char)(*p - '0') <= 9) { f = f * 10 - (*p - '0'); p++; }
        } else {
            while ((unsigned char)(*p - '0') <= 9) { f = f * 10 + (*p - '0'); p++; }
        }

        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
