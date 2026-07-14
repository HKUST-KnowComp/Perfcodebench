#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    return (uint8_t)((((uint32_t)s * a) + ((uint32_t)d * (255 - a)) + 128) >> 8);
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
    size_t pixels = (size_t)width * (size_t)height;
    size_t bytes = pixels * 4;
    uint8_t* out = (uint8_t*)malloc(bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s_ptr = src;
        const uint8_t* d_ptr = dst;
        uint8_t* o_ptr = out;

        for (size_t i = 0; i < pixels; ++i) {
            uint8_t a = s_ptr[3];
            o_ptr[0] = blend_fast(s_ptr[0], d_ptr[0], a);
            o_ptr[1] = blend_fast(s_ptr[1], d_ptr[1], a);
            o_ptr[2] = blend_fast(s_ptr[2], d_ptr[2], a);
            o_ptr[3] = 255;
            s_ptr += 4;
            d_ptr += 4;
            o_ptr += 4;
        }
        hash = checksum_bytes(out, bytes);
    }
    free(out);
    return hash;
}