#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/* 
 * The original formula: (s * a + d * (255 - a) + 127) / 255
 * To maintain exact correctness, we must use the same integer division.
 * We can optimize the division by 255 using: (x + 1 + (x >> 8)) >> 8 is not quite right for all x.
 * The most robust way to optimize (x / 255) is (x + 1 + (x >> 8)) >> 8 for small x, 
 * but since we need exactness, we use the property that (x / 255) 
 * can be computed as (x + 1 + (x >> 8)) >> 8 only if we are careful. 
 * Actually, for the range of uint32_t, (x + 1 + (x >> 8)) >> 8 is a common trick, 
 * but let's use the standard fast division: (x + 128) / 255 is not it. 
 * Let's use: (x + 1 + (x >> 8)) >> 8 is for x/255. 
 * Let's just use the compiler's ability to optimize division by a constant if we can.
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
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;

    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint8_t a = src[idx + 3u];
            uint32_t inv_a = 255u - a;
            
            // Unrolling and using local variables to help compiler
            uint32_t r = (uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127u;
            uint32_t g = (uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * inv_a + 127u;
            uint32_t b = (uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * inv_a + 127u;
            
            out[idx]     = (uint8_t)(r / 255u);
            out[idx + 1] = (uint8_t)(g / 255u);
            out[idx + 2] = (uint8_t)(b / 255u);
            out[idx + 3] = 255u;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}