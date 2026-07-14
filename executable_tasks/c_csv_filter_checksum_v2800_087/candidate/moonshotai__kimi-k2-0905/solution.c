#include "interface.h"
#include <stdint.h>

static inline uint32_t parse_u32(const char **s) {
    uint32_t v = 0;
    const char *p = *s;
    while (*p >= '0' && *p <= '9') {
        v = v * 10U + (uint32_t)(*p++ - '0');
    }
    *s = p;
    return v;
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];

        /* skip "r" */
        ++s;
        uint32_t id = parse_u32(&s);
        (void)id;                       /* unused */

        ++s;                            /* skip comma */
        uint32_t a = parse_u32(&s);
        (void)a;

        ++s;
        uint32_t b = parse_u32(&s);

        ++s;
        uint32_t c = parse_u32(&s);

        ++s;                            /* skip comma before flag */
        uint32_t flag_word = *(const uint32_t *)s;
        uint64_t v = (uint64_t)(b + c + 25);
        if (flag_word == 0x006E6F68) {  /* "hon\0" in little-endian */
            v += 29ULL;
        }

        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}