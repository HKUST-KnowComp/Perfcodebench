#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * The original formula is: (s * a + d * (255 - a) + 127) / 255
 * This can be rewritten as: (s * a + d * 255 - d * a + 127) / 255
 * = d + (a * (s - d) + 127) / 255
 * 
 * To perform fast division by 255: 
 * (x + 127) / 255 is approximately (x + 128) / 255.
 * A common fast approximation for (v / 255) is ((v + 1) + (v >> 8)) >> 8.
 * However, to maintain exact correctness with the baseline's integer division,
 * we use the property that for 0 <= v <= 255*255, (v + 127) / 255 is exactly
 * the same as the rounded result of v/255.0.
 */

static inline uint8_t fast_blend(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a);
    // Exact implementation of (val + 127) / 255
    return (uint8_t)((val + 127u) / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;

    // Pre-allocate a buffer once to avoid repeated malloc/free in the loop
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            
            // Blend RGB
            out[idx]     = fast_blend(src[idx],     dst[idx],     a);
            out[idx + 1u] = fast_blend(src[idx + 1u], dst[idx + 1u], a);
            out[idx + 2u] = fast_blend(src[idx + 2u], dst[idx + 2u], a);
            out[idx + 3u] = 255u;
        }

        // Checksum pass
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