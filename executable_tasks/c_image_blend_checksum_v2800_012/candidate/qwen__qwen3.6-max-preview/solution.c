#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t t = (uint32_t)s * a + (uint32_t)d * (255u - a);
    return (uint8_t)((t * 257u + 32768u) >> 16);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    (void)iters;
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    const uint8_t* s = src;
    const uint8_t* d = dst;

    for (size_t i = 0; i < pixels; ++i) {
        uint8_t a = s[3];
        uint8_t b0 = blend_byte(s[0], d[0], a);
        uint8_t b1 = blend_byte(s[1], d[1], a);
        uint8_t b2 = blend_byte(s[2], d[2], a);

        h ^= b0; h *= prime;
        h ^= b1; h *= prime;
        h ^= b2; h *= prime;
        h ^= 255u; h *= prime;

        s += 4;
        d += 4;
    }
    return h;
}