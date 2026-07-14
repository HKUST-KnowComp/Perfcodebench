#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// Fast integer division for (val / 255)
// Using the approximation: (val * 0x8081) >> 16
// This is a standard trick for 8-bit division by 255.
static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)((val * 0x8081u) >> 16);
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
    size_t num_bytes = (size_t)width * (size_t)height * 4u;
    uint8_t* out = (uint8_t*)malloc(num_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    size_t pixels = (size_t)width * (size_t)height;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        // Unroll loop to process 4 pixels at a time to improve ILP
        for (; i + 3 < pixels; i += 4) {
            for (int p = 0; p < 4; ++p) {
                size_t idx = (i + p) << 2;
                uint8_t a = src[idx + 3];
                out[idx]     = blend_byte_fast(src[idx],     dst[idx],     a);
                out[idx + 1] = blend_byte_fast(src[idx + 1], dst[idx + 1], a);
                out[idx + 2] = blend_byte_fast(src[idx + 2], dst[idx + 2], a);
                out[idx + 3] = 255;
            }
        }
        // Handle remaining pixels
        for (; i < pixels; ++i) {
            size_t idx = i << 2;
            uint8_t a = src[idx + 3];
            out[idx]     = blend_byte_fast(src[idx],     dst[idx],     a);
            out[idx + 1] = blend_byte_fast(src[idx + 1], dst[idx + 1], a);
            out[idx + 2] = blend_byte_fast(src[idx + 2], dst[idx + 2], a);
            out[idx + 3] = 255;
        }
        
        hash = checksum_bytes(out, num_bytes);
    }

    free(out);
    return hash;
}