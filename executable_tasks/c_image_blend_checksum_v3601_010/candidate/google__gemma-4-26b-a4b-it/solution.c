#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/* 
 * The original blend function: 
 * (s * a + d * (255 - a) + 127) / 255 
 * 
 * To maintain exact correctness, we must use the exact integer division.
 * We can optimize the division by 255 using the property: 
 * x / 255 ≈ (x * 257) >> 16 
 * However, for exactness with the +127 offset, we use the standard 
 * integer division or a more precise bit-shift trick if possible.
 * Given the requirement for exactness, we will use the division but 
 * optimize the loop structure and memory management.
 */

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t sa = (uint32_t)s * (uint32_t)a;
    uint32_t da = (uint32_t)d * (uint32_t)(255u - a);
    return (uint8_t)((sa + da + 127u) / 255u);
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
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < num_bytes; i += 4) {
            uint8_t a = src[i + 3];
            out[i]     = blend_byte_fast(src[i],     dst[i],     a);
            out[i + 1] = blend_byte_fast(src[i + 1], dst[i + 1], a);
            out[i + 2] = blend_byte_fast(src[i + 2], dst[i + 2], a);
            out[i + 3] = 255;
        }
        hash = checksum_bytes(out, num_bytes);
    }

    free(out);
    return hash;
}