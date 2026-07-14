#include "interface.h"
#include <stdint.h>

static inline int parse_int(const char** p) {
    int sign = 1;
    const char* s = *p;
    if (*s == '-') {
        sign = -1;
        ++s;
    }
    int val = 0;
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        ++s;
    }
    *p = s;
    return sign * val;
}

static inline void skip_int(const char** p) {
    const char* s = *p;
    if (*s == '-') ++s;
    while (*s >= '0' && *s <= '9') ++s;
    *p = s;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        if (*p == 'r') ++p;
        skip_int(&p);
        if (*p == ',') ++p;
        skip_int(&p);
        if (*p == ',') ++p;
        int b = parse_int(&p);
        if (*p == ',') ++p;
        int c = parse_int(&p);
        if (*p == ',') ++p;
        uint64_t v = (uint64_t)(b + c + 11);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && p[3] == '\0') {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}
