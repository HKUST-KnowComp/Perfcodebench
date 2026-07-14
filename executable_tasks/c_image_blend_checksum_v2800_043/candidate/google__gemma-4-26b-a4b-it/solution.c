#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// The original formula: (s*a + d*(255-a) + 127) / 255
// This is equivalent to: (s*a + d*255 - d*a + 127) / 255
// = (a*(s-d) + d*255 + 127) / 255
// = d + (a*(s-d) + 127) / 255
// To maintain exact correctness, we must use the exact integer division.

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)(val / 255u);
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
            uint32_t a = src[idx + 3u];
            uint32_t inv_a = 255u - a;
            
            // Unrolling and using 32-bit math to reduce dependencies
            uint32_t r = (uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127u;
            uint32_t g = (uint32_t)src[idx + 1u] * a + (uint32_t)dst[idx + 1u] * inv_a + 127u;
            uint32_t b = (uint32_t)src[idx + 2u] * a + (uint32_t)dst[idx + 2u] * inv_a + 127u;
            
            out[idx]     = (uint8_t)(r / 255u);
            out[idx + 1u] = (uint8_t)(g / 255u);
            out[idx + 2u] = (uint8_t)(b / 255u);
            out[idx + 3u] = 255u;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}