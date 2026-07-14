#include "interface.h"
#include <stdint.h>

static inline int parse_int(const char** p) {
    int val = 0;
    while (**p >= '0' && **p <= '9') {
        val = val * 10 + (**p - '0');
        (*p)++;
    }
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char* p = rows[i];
        if (*p == 'r') p++;
        parse_int(&p); // skip id
        if (*p == ',') p++;
        parse_int(&p); // skip a
        if (*p == ',') p++;
        int b = parse_int(&p);
        if (*p == ',') p++;
        int c = parse_int(&p);
        if (*p == ',') p++;
        
        uint64_t v = (uint64_t)(b + c + 22);
        // Check for "hot" without strcmp: 'h' is 104, 'o' is 111, 't' is 116
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}