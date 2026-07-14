#include "interface.h"
#include <stdint.h>

static inline int parse_int(const char **s) {
    int val = 0;
    while (**s >= '0' && **s <= '9') {
        val = val * 10 + (**s - '0');
        (*s)++;
    }
    return val;
}

uint64_t run(const char *const *rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        p++; /* skip 'r' */
        int id = parse_int(&p); p++; /* skip comma */
        int a  = parse_int(&p); p++; /* skip comma */
        int b  = parse_int(&p); p++; /* skip comma */
        int c  = parse_int(&p); p++; /* skip comma */
        
        uint64_t v = (uint64_t)(b + c + 25);
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == 0 || p[3] == '\n')) {
            v += 29ULL;
        }
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}