#include "interface.h"
#include <stdint.h>
#include <ctype.h>

static inline int parse_int(const char **p) {
    int val = 0;
    while (isdigit((unsigned char)**p)) {
        val = val * 10 + (**p - '0');
        (*p)++;
    }
    (*p)++; // Skip trailing comma
    return val;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        p++; // Skip leading 'r'
        parse_int(&p); // Parse unused id, matches original behavior
        parse_int(&p); // Parse unused a, matches original behavior
        int b = parse_int(&p);
        int c = parse_int(&p);
        uint64_t v = (uint64_t)(b + c + 17);
        // Exact "hot" flag check matching original strcmp/sscanf behavior
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') {
            if (p[3] == '\0' || isspace((unsigned char)p[3])) {
                v += 29ULL;
            }
        }
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}