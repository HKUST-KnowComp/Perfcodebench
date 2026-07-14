#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Optimized blend using the property: (v + 127) / 255  approx equals (v + 1 + (v >> 8)) >> 8
// Specifically for: (s*a + d*(255-a) + 127) / 255
static inline uint8_t blend_fast(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t t = s * a + d * (255u - a) + 127u;
    return (uint8_t)((t + (t >> 8) + 1) >> 8);
}

// FNV-1a hash implementation
static inline uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Process pixels in a tight loop
        // Compilers can vectorize this effectively if the math is simple
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint32_t s0 = src[idx];
            uint32_t s1 = src[idx + 1];
            uint32_t s2 = src[idx + 2];
            uint32_t a  = src[idx + 3];
            
            uint32_t d0 = dst[idx];
            uint32_t d1 = dst[idx + 1];
            uint32_t d2 = dst[idx + 2];

            // Standard blend: (s*a + d*(255-a) + 127) / 255
            // We use the exact formula provided in the baseline for correctness
            out[idx]     = (uint8_t)((s0 * a + d0 * (255u - a) + 127u) / 255u);
            out[idx + 1] = (uint8_t)((s1 * a + d1 * (255u - a) + 127u) / 255u);
            out[idx + 2] = (uint8_t)((s2 * a + d2 * (255u - a) + 127u) / 255u);
            out[idx + 3] = 255u;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}