#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Fast approximation of (s * a + d * (255 - a) + 127) / 255
 * The formula (x + 127) / 255 can be approximated by ((x + 127) * 0x8081) >> 23
 * However, for exactness as per the baseline, we use the property:
 * (val + 127) / 255 is equivalent to (val + 127 + 255/2) / 255? No.
 * Let's use the standard fast division for 255: (x + 1 + (x >> 8)) >> 8 is for x/255.
 * To maintain exact correctness with the baseline's `(s*a + d*(255-a) + 127) / 255`,
 * we can use the identity: (n + 127) / 255.
 */

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a);
    // Exact division by 255: (val + 127) / 255
    // Using the property that /255 is approx * (1/255)
    // For small values, we can use the division directly as compilers optimize /255
    // into a multiply and shift (magic numbers).
    return (uint8_t)((val + 127u) / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < total_bytes; i += 4) {
            uint8_t a = src[i + 3];
            out[i]     = blend_byte_fast(src[i],     dst[i],     a);
            out[i + 1] = blend_byte_fast(src[i + 1], dst[i + 1], a);
            out[i + 2] = blend_byte_fast(src[i + 2], dst[i + 2], a);
            out[i + 3] = 255;
        }

        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_bytes; ++i) {
            current_hash ^= (uint64_t)out[i];
            current_hash *= FNV_PRIME;
        }
        hash = current_hash;
    }

    free(out);
    return hash;
}