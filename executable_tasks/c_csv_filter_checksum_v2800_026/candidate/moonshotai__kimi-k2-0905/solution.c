#include "interface.h"

#include <stdint.h>

static inline uint64_t parse_u64(const char **p) {
    uint64_t v = 0;
    while (**p >= '0' && **p <= '9') {
        v = v * 10ULL + (uint64_t)(*(*p)++ - '0');
    }
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *p = rows[i];

        // Skip "r" prefix
        ++p;
        uint64_t id  = parse_u64(&p); (void)id; // unused, but parsed for correctness
        ++p; // ','
        uint64_t a   = parse_u64(&p); (void)a;   // unused
        ++p; // ','
        uint64_t b   = parse_u64(&p);
        ++p; // ','
        uint64_t c   = parse_u64(&p);
        ++p; // ','

        // Check flag: "hot" is 3 bytes followed by ',' or '\0'
        uint64_t flag_bonus = 0;
        if (p[0] == 'h' && p[1] == 'o' && p[2] == 't' && (p[3] == ',' || p[3] == '\0')) {
            flag_bonus = 29ULL;
        }

        uint64_t v = b + c + 10ULL + flag_bonus;
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}