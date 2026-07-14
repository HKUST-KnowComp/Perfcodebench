#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Fast approximation of (s * a + d * (255 - a) + 127) / 255
 * The formula (val + 127) / 255 can be approximated as (val + 1 + (val >> 8)) >> 8
 * for 0 <= val <= 255*255.
 * Specifically, (s*a + d*(255-a) + 127) / 255 is equivalent to
 * (s*a + d*255 - d*a + 127) / 255 = d + (a*(s-d) + 127) / 255.
 */
static inline uint8_t fast_blend(uint8_t s, uint8_t d, uint8_t a) {
    int32_t diff = (int32_t)s - (int32_t)d;
    int32_t prod = diff * (int32_t)a;
    // We need (prod + 127) / 255. 
    // For positive prod: (prod + 127 + 255/2) / 255 approx
    // A more robust way for the specific range of RGBA blending:
    // result = d + ((prod + 127) / 255)
    // Using the property: (x + 127) / 255 ≈ (x + 128) >> 8 is not quite right.
    // Let's use the standard fast blend: ((s - d) * a) / 255 + d
    // (s*a + d*(255-a) + 127) / 255 = (a*(s-d) + 255*d + 127) / 255 = d + (a*(s-d) + 127) / 255
    
    int32_t num = prod + 127;
    // Division by 255: (num + 1 + (num >> 8)) >> 8 for positive num
    // Since num can be negative, we handle it carefully.
    if (num >= 0) {
        return (uint8_t)(d + ((num + 1 + (num >> 8)) >> 8));
    } else {
        // For negative num, we need to maintain the floor/round behavior of the original
        // Original: (num) / 255. In C, -127 / 255 = 0.
        // To match (uint32_t) division, we use the property that the original code
        // cast everything to uint32_t, so it was actually doing unsigned wrap-around
        // before dividing. 
        // Let's revert to a slightly safer but still fast version.
        return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u) / 255u);
    }
}

// To ensure exact correctness with the baseline's (uint32_t) division, 
// we use the division but optimize the loop and memory.

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    const uint64_t prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint32_t a = src[idx + 3u];
            uint32_t inv_a = 255u - a;
            
            out[idx]     = (uint8_t)(((uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127u) / 255u);
            out[idx + 1] = (uint8_t)(((uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * inv_a + 127u) / 255u);
            out[idx + 2] = (uint8_t)(((uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * inv_a + 127u) / 255u);
            out[idx + 3] = 255u;
        }

        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_bytes; ++i) {
            current_hash ^= (uint64_t)out[i];
            current_hash *= prime;
        }
        hash = current_hash;
    }

    free(out);
    return hash;
}