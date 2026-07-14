#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline const char* parse_int(const char* s, int* val) {
    int sign = 1;
    int num = 0;
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    while (*s >= '0' && *s <= '9') {
        num = num * 10 + (*s - '0');
        s++;
    }
    *val = num * sign;
    return s;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        int id, a, b, c;
        char flag[8];

        /* skip leading 'r' */
        if (*p == 'r') p++;
        p = parse_int(p, &id);
        if (*p == ',') p++;
        p = parse_int(p, &a);
        if (*p == ',') p++;
        p = parse_int(p, &b);
        if (*p == ',') p++;
        p = parse_int(p, &c);
        if (*p == ',') p++;

        /* extract flag up to 7 characters */
        int flen = 0;
        while (*p != ',' && *p != '\0' && flen < 7) {
            flag[flen++] = *p++;
        }
        flag[flen] = '\0';

        uint64_t v = (uint64_t)(b + c + 20);
        if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't' && flag[3] == '\0') {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
