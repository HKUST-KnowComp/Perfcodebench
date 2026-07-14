#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// The original formula: (s * a + d * (255 - a) + 127) / 255
// We can optimize the division by 255 using: (x + 1 + (x >> 8)) >> 8
// Let's check: if x = 255*255 + 127 = 65152. (65152 + 1 + 254) >> 8 = 65407 >> 8 = 255. Correct.
// If x = 0. (0 + 1 + 0) >> 8 = 0. Correct.

static inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)((val + 1 + (val >> 8)) >> 8);
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
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;
    
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint8_t a = src[idx + 3];
            
            // Unrolled blending for better instruction pipelining
            uint32_t v0 = (uint32_t)src[idx] * a + (uint32_t)dst[idx] * (255u - a) + 127u;
            out[idx] = (uint8_t)((v0 + 1 + (v0 >> 8)) >> 8);
            
            uint32_t v1 = (uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * (255u - a) + 127u;
            out[idx + 1] = (uint8_t)((v1 + 1 + (v1 >> 8)) >> 8);
            
            uint32_t v2 = (uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * (255u - a) + 127u;
            out[idx + 2] = (uint8_t)((v2 + 1 + (v2 >> 8)) >> 8);
            
            out[idx + 3] = 255u;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}