#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * The original blend formula: ((s * a + d * (255 - a) + 127) / 255)
 * can be approximated as: ((s * a + d * (255 - a) + 127) + ((s * a + d * (255 - a) + 127) >> 8)) >> 8
 * However, for exactness with the provided baseline's rounding, we use the standard fast 
 * integer division by 255: (val + 1 + (val >> 8)) >> 8 is a common approximation,
 * but the baseline uses (val + 127) / 255. 
 * 
 * Let's use the property: (x + 127) / 255 is equivalent to (x + 127) * 0x01010101 >> 24 (approx).
 * Actually, the most performant way to maintain exact correctness for (x + 127) / 255 
 * is to use the formula: ((x + 127) + ((x + 127) >> 8)) >> 8 is not quite right.
 * 
 * Given the constraints and the need for speed, we will use a pre-allocated buffer 
 * to avoid malloc/free in the loop and process the checksum on the fly.
 */

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        while (i < pixels) {
            size_t idx = i * 4u;
            uint32_t a = src[idx + 3u];
            uint32_t inv_a = 255u - a;

            // Blend R, G, B
            for (int c = 0; c < 3; ++c) {
                uint32_t val = (uint32_t)src[idx + c] * a + (uint32_t)dst[idx + c] * inv_a;
                out[idx + c] = (uint8_t)((val + 127u) / 255u);
            }
            out[idx + 3u] = 255u;
            i++;
        }

        // Checksum pass
        uint64_t current_hash = FNV_OFFSET_BASIS;
        for (size_t j = 0; j < total_bytes; ++j) {
            current_hash ^= (uint64_t)out[j];
            current_hash *= FNV_PRIME;
        }
        hash = current_hash;
    }

    free(out);
    return hash;
}