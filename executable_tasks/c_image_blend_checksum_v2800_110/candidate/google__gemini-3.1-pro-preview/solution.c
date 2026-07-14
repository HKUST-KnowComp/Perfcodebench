#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Optimized blend using the property: (v + 127) / 255 is approximately (v + 128 + (v + 128) >> 8) >> 8
// For exactness with (s*a + d*(255-a) + 127) / 255:
static inline uint8_t blend_byte_fast(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t v = s * a + d * (255u - a) + 127u;
    return (uint8_t)((v + (v >> 8)) >> 8);
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
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        // Unroll loop to process pixels more efficiently
        for (; i < pixels; ++i) {
            size_t idx = i << 2;
            uint32_t s0 = src[idx];
            uint32_t s1 = src[idx + 1];
            uint32_t s2 = src[idx + 2];
            uint32_t a  = src[idx + 3];
            
            uint32_t d0 = dst[idx];
            uint32_t d1 = dst[idx + 1];
            uint32_t d2 = dst[idx + 2];

            out[idx]     = blend_byte_fast(s0, d0, a);
            out[idx + 1] = blend_byte_fast(s1, d1, a);
            out[idx + 2] = blend_byte_fast(s2, d2, a);
            out[idx + 3] = 255u;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}