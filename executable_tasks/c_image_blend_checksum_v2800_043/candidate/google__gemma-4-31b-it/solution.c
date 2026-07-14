#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Fast approximation of (val + 127) / 255
 * (val + 127) * 0x8081 >> 23 is a very close approximation for 0 <= val <= 65280
 */
static inline uint8_t fast_div255(uint32_t val) {
    return (uint8_t)(((val + 127) * 0x8081) >> 23);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint32_t a = src[idx + 3];
            uint32_t inv_a = 255u - a;

            // Blend formula: (s*a + d*(255-a) + 127) / 255
            out[idx]     = fast_div255((uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a);
            out[idx + 1] = fast_div255((uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * inv_a);
            out[idx + 2] = fast_div255((uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * inv_a);
            out[idx + 3] = 255u;
        }

        // Checksum
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