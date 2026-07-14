#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_MULT = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        // Parse u from "u=...&"
        p += 2;
        int u = 0;
        while (*p != '&') {
            u = u * 10 + (*p - '0');
            p++;
        }
        // Parse s from "&score=...&"
        p += 7;
        int s = 0;
        while (*p != '&') {
            s = s * 10 + (*p - '0');
            p++;
        }
        // Parse f from "&flag=...\0"
        p += 6;
        int f = 0;
        while (*p != '\0') {
            f = f * 10 + (*p - '0');
            p++;
        }
        // Exact same hash calculation as baseline
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= FNV_MULT;
    }
    return hash;
}