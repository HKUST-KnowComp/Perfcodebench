#include "interface.h"
#include <stdint.h>

static inline uint64_t parse_u64(const char *s, const char **end) {
    uint64_t v = 0;
    while (*s >= '0' && *s <= '9') {
        v = v * 10ULL + (uint64_t)(*s - '0');
        ++s;
    }
    *end = s;
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];
        if (*p++ != 'r') continue;          // skip leading 'r'

        uint64_t id = parse_u64(p, &p);      // id
        p++;                                 // skip comma
        uint64_t a  = parse_u64(p, &p);      // a
        p++;
        uint64_t b  = parse_u64(p, &p);      // b
        p++;
        uint64_t c  = parse_u64(p, &p);      // c
        p++;

        uint64_t v = b + c + 28ULL;
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't') v += 29ULL;

        hash ^= (v + 1ULL);
        hash *= 1099511628211ULL;
    }
    return hash;
}