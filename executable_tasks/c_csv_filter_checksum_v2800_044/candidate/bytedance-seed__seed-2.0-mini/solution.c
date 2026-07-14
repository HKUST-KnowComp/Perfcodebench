#include "interface.h"
#include <stdint.h>

static int parse_int(const char** s) {
    int sign = 1;
    int val = 0;
    if (**s == '-') {
        sign = -1;
        (*s)++;
    }
    while (**s != ',') {
        val = val * 10 + (**s - '0');
        (*s)++;
    }
    (*s)++;
    return val * sign;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        s++; // Skip leading 'r'
        
        int id = parse_int(&s);
        int a = parse_int(&s);
        int b = parse_int(&s);
        int c = parse_int(&s);
        
        uint64_t v = (uint64_t)(b + c + 28);
        if (s[0] == 'h' && s[1] == 'o' && s[2] == 't' && s[3] == '\0') {
            v += 29ULL;
        }
        
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    
    return hash;
}