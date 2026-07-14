#include "interface.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

static inline const char* skip_whitespace(const char* p) {
    while (*p == ' ' || *p == '\t') ++p;
    return p;
}

static inline const char* parse_int(const char* p, int* val) {
    p = skip_whitespace(p);
    bool neg = false;
    if (*p == '-') {
        neg = true;
        ++p;
    } else if (*p == '+') {
        ++p;
    }
    int v = 0;
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (int)(*p - '0');
        ++p;
    }
    *val = neg ? -v : v;
    return p;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        // Expect leading 'r'
        if (*p == 'r') ++p;
        int id, a, b, c;
        p = parse_int(p, &id);
        // comma
        if (*p == ',') ++p;
        p = parse_int(p, &a);
        if (*p == ',') ++p;
        p = parse_int(p, &b);
        if (*p == ',') ++p;
        p = parse_int(p, &c);
        if (*p == ',') ++p;
        // Now flag field
        p = skip_whitespace(p);
        const char* flag_start = p;
        while (*p != '\0') ++p;
        size_t flag_len = (size_t)(p - flag_start);

        uint64_t v = (uint64_t)(b + c + 19);
        if (flag_len == 3 && flag_start[0] == 'h' && flag_start[1] == 'o' && flag_start[2] == 't') {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= multiplier;
    }
    return hash;
}
