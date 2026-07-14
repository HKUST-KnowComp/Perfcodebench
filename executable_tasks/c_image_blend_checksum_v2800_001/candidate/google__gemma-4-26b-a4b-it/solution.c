#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// Optimized blend function using bit-shifts to avoid division
// The original formula: (s*a + d*(255-a) + 127) / 255
// can be approximated/implemented efficiently.
// However, to ensure EXACT correctness as per the requirement, 
// we must be careful. The original uses integer division.
// A common trick for (x + 127) / 255 is (x + 128 + (x >> 8)) >> 8, 
// but since we need EXACT match, we use a slightly faster way to handle the math.

static inline uint8_t blend_byte_fast(uint32_t s, uint32_t d, uint32_t a) {
    return (uint8_t)(((s * a) + (d * (255u - a)) + 127u) / 255u);
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
    size_t num_pixels = (size_t)width * (size_t)height;
    size_t total_bytes = num_pixels * 4u;
    uint64_t hash = 0;

    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Process pixels in a way that encourages auto-vectorization
        for (size_t i = 0; i < num_pixels; ++i) {
            size_t idx = i << 2; // i * 4
            uint32_t a = src[idx + 3];
            uint32_t inv_a = 255u - a;

            out[idx]     = (uint8_t)(((uint32_t)src[idx]     * a + (uint32_t)dst[idx]     * inv_a + 127u) / 255u);
            out[idx + 1] = (uint8_t)(((uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * inv_a + 127u) / 255u);
            out[idx + 2] = (uint8_t)(((uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * inv_a + 127u) / 255u);
            out[idx + 3] = 255u;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}