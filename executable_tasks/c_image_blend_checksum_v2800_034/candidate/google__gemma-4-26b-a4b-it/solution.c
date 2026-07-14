#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/* 
 * The original blend function: 
 * (s * a + d * (255 - a) + 127) / 255 
 * 
 * We can optimize the division by 255 using the property: 
 * x / 255 ≈ (x + (x >> 8) + 1) >> 8 
 * However, to ensure exact correctness with the original formula, 
 * we must be careful. 
 * 
 * Let's re-examine the original: 
 * res = (s*a + d*(255-a) + 127) / 255
 * 
 * Since we need EXACT correctness, we must match the integer division behavior.
 */

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
    (void)width;
    (void)height;
    size_t num_bytes = (size_t)width * (size_t)height * 4u;
    size_t pixels = num_bytes / 4u;
    uint64_t hash = 0;

    uint8_t* out = (uint8_t*)malloc(num_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint32_t a = src[idx + 3];
            uint32_t inv_a = 255u - a;
            
            // We use uint32_t to avoid repeated casting and allow compiler to vectorize
            out[idx]     = (uint8_t)(((uint32_t)src[idx]     * a + (uint32_t)dst[idx]     * inv_a + 127u) / 255u);
            out[idx + 1] = (uint8_t)(((uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * inv_a + 127u) / 255u);
            out[idx + 2] = (uint8_t)(((uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * inv_a + 127u) / 255u);
            out[idx + 3] = 255u;
        }
        hash = checksum_bytes(out, num_bytes);
    }

    free(out);
    return hash;
}