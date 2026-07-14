#include "interface.h"
#include <stdint.h>
#include <string.h>

static inline int is_hex_4(uint32_t v) {
    uint32_t a = v + 0x33333333U;
    uint32_t b = a & 0x88888888U;
    return (b == 0);
}

static inline int is_hex_8(uint64_t v) {
    uint64_t a = v + 0x3333333333333333ULL;
    uint64_t b = a & 0x8888888888888888ULL;
    return (b == 0);
}

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < row_count; ++i) {
        const unsigned char *s = (const unsigned char *)rows[i];
        uint64_t v = 0;
        if (__builtin_expect(__builtin_strlen((const char *)s) == 15, 1)) {
            uint64_t lo, hi;
            memcpy(&lo, s, 8);
            memcpy(&hi, s + 7, 8);
            if ((lo & 0x0000FF0000000000ULL) == ((uint64_t)'-' << 40) &&
                (lo & 0x000000000000FF00ULL) == ((uint64_t)'-' << 8)  &&
                is_hex_8(lo & 0xFF00FFFFFFFF00ULL) &&
                is_hex_4((uint32_t)(hi >> 32))) {
                v = (uint64_t)(s[0] + s[7]);
            }
        }
        hash ^= (v + 1ULL);
        hash *= prime;
    }
    return hash;
}