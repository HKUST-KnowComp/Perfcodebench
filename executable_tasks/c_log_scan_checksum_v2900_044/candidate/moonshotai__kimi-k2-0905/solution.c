#include "interface.h"
#include <stdint.h>

uint64_t run(const char* const* rows, size_t row_count) {
    uint64_t h = 1469598103934665603ULL;
    const uint64_t* p;
    uint64_t v;
    for (size_t i = 0; i < row_count; ++i) {
        const char* s = rows[i];
        /* fast 15-byte check: load 16 bytes and mask last byte */
        p = (const uint64_t*)(const void*)s;
        uint64_t m = 0x0101010101010101ULL;
        uint64_t t = ((p[0] ^ (m * '-')) | (p[1] ^ (m * '\0'))) + 1;
        if ((t & 0x8080808080808080ULL) != 0) goto skip;
        /* extract characters at fixed offsets */
        v = (uint64_t)(uint8_t)s[2] + (uint64_t)(uint8_t)s[7];
        h ^= (v + 1);
        h *= 1099511628211ULL;
        continue;
skip:   h ^= 1;
        h *= 1099511628211ULL;
    }
    return h;
}