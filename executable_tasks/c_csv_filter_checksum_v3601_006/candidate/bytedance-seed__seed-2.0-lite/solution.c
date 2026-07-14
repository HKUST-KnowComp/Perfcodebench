#include "interface.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int parse_int(const char **p) {
    int sign = 1;
    int num = 0;
    if (**p == '-') {
        sign = -1;
        (*p)++;
    }
    while (**p >= '0' && **p <= '9') {
        num = num * 10 + (*(*p)++ - '0');
    }
    (*p)++; // Skip trailing comma
    return num * sign;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        p++; // Skip leading 'r' character
        int id = parse_int(&p);
        int a = parse_int(&p);
        int b = parse_int(&p);
        int c = parse_int(&p);
        // Check if flag is "hot" without strcmp overhead
        const int is_hot = (p[0] == 'h' && p[1] == 'o' && p[2] == 't');
        uint64_t v = (uint64_t)(b + c + 25);
        if (is_hot) v += 29ULL;
        hash ^= (v + 1ULL);
        hash *= fnv_prime;
    }
    return hash;
}