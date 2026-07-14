#include "interface.h"

#include <stdint.h>
#include <string.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* row = rows[i];
        const char* p = row;
        if (*p == 'r') ++p;
        // skip id
        while (*p && *p != ',') ++p;
        if (*p == ',') ++p;
        // skip a
        while (*p && *p != ',') ++p;
        if (*p == ',') ++p;
        // parse b
        int b = 0;
        int sign_b = 1;
        if (*p == '-') { sign_b = -1; ++p; }
        else if (*p == '+') { ++p; }
        while (*p >= '0' && *p <= '9') {
            b = b * 10 + (*p - '0');
            ++p;
        }
        b *= sign_b;
        if (*p == ',') ++p;
        // parse c
        int c = 0;
        int sign_c = 1;
        if (*p == '-') { sign_c = -1; ++p; }
        else if (*p == '+') { ++p; }
        while (*p >= '0' && *p <= '9') {
            c = c * 10 + (*p - '0');
            ++p;
        }
        c *= sign_c;
        if (*p == ',') ++p;
        // parse flag (up to 7 chars)
        char flag[8];
        int flen = 0;
        while (flen < 7 && *p && *p != ',') {
            flag[flen++] = *p++;
        }
        flag[flen] = '\0';
        // compute hash
        uint64_t v = (uint64_t)(b + c + 14);
        if (flen == 3 && flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't') {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
