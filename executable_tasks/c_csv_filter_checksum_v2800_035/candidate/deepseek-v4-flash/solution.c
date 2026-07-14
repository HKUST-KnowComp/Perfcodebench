#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        // skip leading 'r'
        if (*p == 'r') ++p;
        // skip id digits
        while (*p >= '0' && *p <= '9') ++p;
        // skip comma
        if (*p == ',') ++p;
        // skip a digits
        while (*p >= '0' && *p <= '9') ++p;
        if (*p == ',') ++p;
        // parse b
        int b = 0;
        while (*p >= '0' && *p <= '9') { b = b * 10 + (*p - '0'); ++p; }
        if (*p == ',') ++p;
        // parse c
        int c = 0;
        while (*p >= '0' && *p <= '9') { c = c * 10 + (*p - '0'); ++p; }
        if (*p == ',') ++p;
        // parse flag (up to 7 chars)
        char flag[8];
        int flag_len = 0;
        while (*p && *p != '\n' && *p != '\r' && flag_len < 7) {
            flag[flag_len++] = *p;
            ++p;
        }
        flag[flag_len] = '\0';
        // compute v
        uint64_t v = (uint64_t)(b + c + 19);
        if (flag_len == 3 && flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't') {
            v += 29;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
