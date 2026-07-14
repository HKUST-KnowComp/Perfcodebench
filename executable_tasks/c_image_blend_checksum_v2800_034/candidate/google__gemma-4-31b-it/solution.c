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
 * Let's use the identity: (x + 127) / 255 = floor((x + 127) * (1/255))
 * 1/255 is approx 0x01010101 / 2^32.
 * 
 * To ensure exact match with the baseline's (s*a + d*(255-a) + 127) / 255:
 * we can use the fact that (val + 127) / 255 is exactly (val + 127 + ((val + 127) / 255)) >> 8
 * but that's recursive. 
 * 
 * A known exact replacement for (n + 127) / 255 for 0 <= n <= 255*255 is:
 * ((n + 127) * 0x8081) >> 23 is very close, but for exactness we use the baseline logic
 * but optimize the loop and memory.
 */

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a);
    return (uint8_t)((val + 127u) / 255u);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    const uint64_t hash_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            
            // Optimization: if alpha is 0, result is dst. If 255, result is src.
            if (a == 0) {
                out[idx] = dst[idx];
                out[idx + 1] = dst[idx + 1];
                out[idx + 2] = dst[idx + 2];
            } else if (a == 255) {
                out[idx] = src[idx];
                out[idx + 1] = src[idx + 1];
                out[idx + 2] = src[idx + 2];
            } else {
                out[idx] = blend_byte_fast(src[idx], dst[idx], a);
                out[idx + 1] = blend_byte_fast(src[idx + 1], dst[idx + 1], a);
                out[idx + 2] = blend_byte_fast(src[idx + 2], dst[idx + 2], a);
            }
            out[idx + 3] = 255u;
        }

        // Inline checksum_bytes
        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_bytes; ++i) {
            current_hash ^= (uint64_t)out[i];
            current_hash *= hash_prime;
        }
        hash = current_hash;
    }

    free(out);
    return hash;
}
