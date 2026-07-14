#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];

        // Skip the initial 'r' and the first integer field (id)
        while (*s != ',') ++s;
        ++s; // skip comma

        // Skip second integer field (a)
        while (*s != ',') ++s;
        ++s; // skip comma

        // Parse third integer field (b) with possible sign
        int b_val = 0;
        int sign = 1;
        if (*s == '-') {
            sign = -1;
            ++s;
        }
        while (*s >= '0' && *s <= '9') {
            b_val = b_val * 10 + (*s - '0');
            ++s;
        }
        b_val *= sign;
        ++s; // skip comma

        // Parse fourth integer field (c) with possible sign
        int c_val = 0;
        sign = 1;
        if (*s == '-') {
            sign = -1;
            ++s;
        }
        while (*s >= '0' && *s <= '9') {
            c_val = c_val * 10 + (*s - '0');
            ++s;
        }
        ++s; // skip comma

        // The rest of the string is the flag field
        const char* flag = s;

        // Compute v exactly as original: b+c (int addition) +13, cast to uint64_t
        uint64_t v = (uint64_t)(b_val + c_val + 13);
        // Check flag == "hot" (null-terminated, exact 3 chars)
        if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}