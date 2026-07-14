#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Fast division by 255 for uint32_t values in range [0, 255*255].
 * The formula (val + 1 + (val >> 8)) >> 8 is a common fast approximation
 * for (val + 127) / 255 for the specific range of blending.
 * However, to ensure exact correctness with the baseline's (s*a + d*(255-a) + 127)/255,
 * we use the property that (x + 127) / 255 is equivalent to (x + 1 + (x >> 8)) >> 8
 * for the range of values produced by the blend formula.
 */
static inline uint8_t fast_blend(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t val = s * a + d * (255u - a);
    return (uint8_t)((val + 127u) / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;

    // Allocate buffer once outside the loop to avoid repeated malloc/free overhead
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        // Process pixels
        for (; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            
            // Blend RGB
            out[idx]     = fast_blend(src[idx],     dst[idx],     a);
            out[idx + 1u] = fast_blend(src[idx + 1u], dst[idx + 1u], a);
            out[idx + 2u] = fast_blend(src[idx + 2u], dst[idx + 2u], a);
            out[idx + 3u] = 255u;
        }

        // Calculate checksum (FNV-1a style as per baseline)
        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t j = 0; j < total_bytes; ++j) {
            current_hash ^= (uint64_t)out[j];
            current_hash *= 1099511628211ULL;
        }
        hash = current_hash;
    }

    free(out);
    return hash;
}