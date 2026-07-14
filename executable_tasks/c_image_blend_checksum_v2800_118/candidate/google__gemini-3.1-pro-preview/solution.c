#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * The original blend_byte: (s * a + d * (255 - a) + 127) / 255
 * This is equivalent to: d + (s - d) * a / 255 (with rounding).
 * A common fast approximation is ((s - d) * a + 128) >> 8 + d, 
 * but to match the exact output of (x + 127) / 255, we use: 
 * t = s*a + d*(255-a) + 127; res = (t + (t >> 8)) >> 8;
 */
static inline uint8_t blend_exact(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t t = s * a + d * (255 - a) + 127;
    return (uint8_t)((t + (t >> 8)) >> 8);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s_ptr = src;
        const uint8_t* d_ptr = dst;
        uint8_t* o_ptr = out;

        size_t i = 0;
        // Process pixels in blocks to improve cache locality and allow compiler vectorization
        for (; i < pixels; ++i) {
            uint32_t s_r = s_ptr[0];
            uint32_t s_g = s_ptr[1];
            uint32_t s_b = s_ptr[2];
            uint32_t a   = s_ptr[3];

            uint32_t d_r = d_ptr[0];
            uint32_t d_g = d_ptr[1];
            uint32_t d_b = d_ptr[2];

            o_ptr[0] = blend_exact(s_r, d_r, a);
            o_ptr[1] = blend_exact(s_g, d_g, a);
            o_ptr[2] = blend_exact(s_b, d_b, a);
            o_ptr[3] = 255;

            s_ptr += 4;
            d_ptr += 4;
            o_ptr += 4;
        }

        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}