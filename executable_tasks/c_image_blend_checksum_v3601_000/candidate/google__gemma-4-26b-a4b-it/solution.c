#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// The original formula: (s * a + d * (255 - a) + 127) / 255
// Can be rewritten as: (s * a + d * 255 - d * a + 127) / 255
// Or more simply: d + (s - d) * a / 255
// To match the exact integer arithmetic of the original: 
// (s * a + d * (255 - a) + 127) / 255
// We use the fast division trick for x / 255 ≈ (x + 1 + (x >> 8)) >> 8

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)((val + (val >> 8)) >> 8);
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
    size_t bytes = pixels * 4u;
    uint64_t hash = 0;

    uint8_t* out = (uint8_t*)malloc(bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint8_t a = src[idx + 3u];
            
            // Manual inlining of blend_byte to ensure compiler optimization
            uint32_t val0 = (uint32_t)src[idx] * a + (uint32_t)dst[idx] * (255u - a) + 127u;
            out[idx] = (uint8_t)((val0 + (val0 >> 8)) >> 8);
            
            uint32_t val1 = (uint32_t)src[idx + 1u] * a + (uint32_t)dst[idx + 1u] * (255u - a) + 127u;
            out[idx + 1u] = (uint8_t)((val1 + (val1 >> 8)) >> 8);
            
            uint32_t val2 = (uint32_t)src[idx + 2u] * a + (uint32_t)dst[idx + 2u] * (255u - a) + 127u;
            out[idx + 2u] = (uint8_t)((val2 + (val2 >> 8)) >> 8);
            
            out[idx + 3u] = 255u;
        }
        hash = checksum_bytes(out, bytes);
    }

    free(out);
    return hash;
}