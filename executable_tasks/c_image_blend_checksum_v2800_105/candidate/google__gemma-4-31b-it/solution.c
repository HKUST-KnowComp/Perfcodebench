#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Fast approximation of (s * a + d * (255 - a) + 127) / 255
 * The formula (val + 127) / 255 can be approximated as (val + 128) / 255.
 * A common fast division by 255 for uint32_t is: 
 * ((val + 1) + ((val + 1) >> 8)) >> 8
 * However, to maintain exact correctness with the baseline's (x + 127) / 255,
 * we use the property that (x + 127) / 255 is equivalent to (x + 127 + 1) / 256 
 * for most cases, but for exactness we use the standard fast 255 div: 
 * div255(x) = ((x + 1) + ((x + 1) >> 8)) >> 8
 */
static inline uint8_t fast_blend(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    // Exact division by 255 using the property: 
    // n / 255 = (n + 1 + (n >> 8)) >> 8 for n <= 255*255
    // For our range (max 255*255 + 127), we need to be careful.
    // The most reliable fast way for this specific range is:
    return (uint8_t)(((val + 1) + ((val + 1) >> 8)) >> 8);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    (void)width;
    (void)height;
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;

    // Pre-allocate buffer once to avoid repeated malloc/free in the loop
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Blend pixels
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            out[idx]     = fast_blend(src[idx],     dst[idx],     a);
            out[idx + 1] = fast_blend(src[idx + 1], dst[idx + 1], a);
            out[idx + 2] = fast_blend(src[idx + 2], dst[idx + 2], a);
            out[idx + 3] = 255u;
        }

        // Checksum the result
        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_bytes; ++i) {
            current_hash ^= (uint64_t)out[i];
            current_hash *= 1099511628211ULL;
        }
        hash = current_hash;
    }

    free(out);
    return hash;
}