#include "interface.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static inline const char* skip_spaces(const char* p) {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '\f' || *p == '\v') {
        ++p;
    }
    return p;
}

static inline const char* skip_int(const char* p) {
    if (*p == '-' || *p == '+') {
        ++p;
    }
    while (*p >= '0' && *p <= '9') {
        ++p;
    }
    return p;
}

static inline int parse_int(const char** pp) {
    const char* p = *pp;
    int sign = 1;
    if (*p == '-') {
        sign = -1;
        ++p;
    } else if (*p == '+') {
        ++p;
    }
    int val = 0;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        ++p;
    }
    *pp = p;
    return sign * val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        ++p; // skip 'r'
        p = skip_spaces(p);
        p = skip_int(p); // id
        if (*p == ',') ++p; // skip ','
        p = skip_spaces(p);
        p = skip_int(p); // a
        if (*p == ',') ++p; // skip ','
        p = skip_spaces(p);
        int b = parse_int(&p);
        if (*p == ',') ++p; // skip ','
        p = skip_spaces(p);
        int c = parse_int(&p);
        if (*p == ',') ++p; // skip ','
        p = skip_spaces(p);
        uint64_t v = (uint64_t)(b + c + 28);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            char d = p[3];
            if (d == '\0' || d == ' ' || d == '\t' || d == '\n' || d == '\r' || d == '\f' || d == '\v') {
                v += 29ULL;
            }
        }
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}
