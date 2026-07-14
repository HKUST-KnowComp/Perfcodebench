#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Fast approximation of (s * a + d * (255 - a) + 127) / 255
 * The formula (s * a + d * (255 - a) + 127) / 255 is equivalent to
 * d + ((s - d) * a + 127) / 255.
 * 
 * Using the property that (x + 127) / 255 is approximately (x + 128) * 2^32 / (255 * 2^32),
 * we can use a fast division approximation.
 * However, for exact correctness as per the baseline, we use the standard fast
 * integer division trick: (val + 127) / 255 == (val + 127 + 128) >> 8 is not quite right.
 * The exact formula is: (val + 127) / 255.
 * 
 * To maintain exact correctness with the baseline's `(s*a + d*(255-a) + 127) / 255`,
 * we can use the identity: (x + 127) / 255 = (x + 127 + 128) / 256 is not exact.
 * The most reliable fast way for 255 division is `((uint32_t)val * 0x01010101) >> 32` 
 * but that's for different ranges. 
 * 
 * Let's use the property: (s*a + d*(255-a) + 127) / 255 = (a*(s-d) + 255*d + 127) / 255
 * = d + (a*(s-d) + 127) / 255.
 * 
 * For the checksum, we can process the data in a way that avoids the malloc/free cycle.
 */

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t res = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)(res / 255u);
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
        for (size_t i = 0; i < total_bytes; i += 4) {
            uint8_t a = src[i + 3];
            // We can optimize the blend_byte calls
            // Using the baseline formula exactly to ensure correctness
            out[i]     = blend_byte_fast(src[i],     dst[i],     a);
            out[i + 1] = blend_byte_fast(src[i + 1], dst[i + 1], a);
            out[i + 2] = blend_byte_fast(src[i + 2], dst[i + 2], a);
            out[i + 3] = 255u;
        }

        // Inline checksum_bytes
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
