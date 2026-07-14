#include "interface.h"
#include <stdint.h>

static inline int parse_int(const char **s) {
    int n = 0;
    const char *p = *s;
    while (*p >= '0' && *p <= '9') {
        n = n * 10 + (*p++ - '0');
    }
    *s = p;
    return n;
}

static inline int match_hot(const char *s) {
    return (s[0] == 'h' && s[1] == 'o' && s[2] == 't');
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        while (*p++ != ',');               // skip "r<int>,"
        parse_int(&p);                       // skip a
        ++p;                                 // skip ','
        int b = parse_int(&p);
        ++p;                                 // skip ','
        int c = parse_int(&p);
        uint64_t v = (uint64_t)(b + c + 22);
        while (*p++ != ',');                 // skip last int
        if (match_hot(p)) v += 29;
        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}