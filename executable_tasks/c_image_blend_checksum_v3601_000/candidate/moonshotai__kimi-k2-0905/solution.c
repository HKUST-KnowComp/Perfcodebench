#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
    return (uint8_t)(((uint32_t)s * a + (uint32_t)d * (255u - a) + 127u) / 255u);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t bytes = (size_t)width * (size_t)height * 4u;
    uint8_t* out = (uint8_t*)malloc(bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s = src;
        const uint8_t* d = dst;
        uint8_t* o = out;
        const uint8_t* end = out + bytes;

        while (o < end) {
            uint32_t a0 = s[3];
            o[0] = blend_byte(s[0], d[0], a0);
            o[1] = blend_byte(s[1], d[1], a0);
            o[2] = blend_byte(s[2], d[2], a0);
            o[3] = 255u;
            s += 4; d += 4; o += 4;

            uint32_t a1 = s[3];
            o[0] = blend_byte(s[0], d[0], a1);
            o[1] = blend_byte(s[1], d[1], a1);
            o[2] = blend_byte(s[2], d[2], a1);
            o[3] = 255u;
            s += 4; d += 4; o += 4;

            uint32_t a2 = s[3];
            o[0] = blend_byte(s[0], d[0], a2);
            o[1] = blend_byte(s[1], d[1], a2);
            o[2] = blend_byte(s[2], d[2], a2);
            o[3] = 255u;
            s += 4; d += 4; o += 4;

            uint32_t a3 = s[3];
            o[0] = blend_byte(s[0], d[0], a3);
            o[1] = blend_byte(s[1], d[1], a3);
            o[2] = blend_byte(s[2], d[2], a3);
            o[3] = 255u;
            s += 4; d += 4; o += 4;
        }
        hash = checksum_bytes(out, bytes);
    }
    free(out);
    return hash;
}