#include "interface.h"
#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t hash_mul = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* ptr = rows[i];
        // Parse u: match original p0+2 offset into first token
        ptr += 2;
        int u = 0;
        while (*ptr != '&') {
            u = u * 10 + (*ptr - '0');
            ptr++;
        }
        // Parse s: match original p1+6 offset into second token
        ptr++;
        ptr += 6;
        int s = 0;
        while (*ptr != '&') {
            s = s * 10 + (*ptr - '0');
            ptr++;
        }
        // Parse f: match original p2+5 offset into third token
        ptr++;
        ptr += 5;
        int f = 0;
        while (*ptr != '\0') {
            f = f * 10 + (*ptr - '0');
            ptr++;
        }
        // Exact original value and hash calculation preserved
        uint64_t v = (uint64_t)((u & 2047) + s + f * 19);
        hash ^= (v + 1ULL);
        hash *= hash_mul;
    }
    return hash;
}