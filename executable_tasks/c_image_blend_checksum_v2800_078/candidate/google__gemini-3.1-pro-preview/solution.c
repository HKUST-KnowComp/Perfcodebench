#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// The original blend_byte: (s * a + d * (255 - a) + 127) / 255
// This is equivalent to: d + (s - d) * a / 255 (with rounding)
// A common fast approximation is ((s - d) * a + 128) >> 8 + d, 
// but we must match the exact output of the original formula.
static inline uint8_t blend_byte_exact(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t s32 = s;
    uint32_t d32 = d;
    uint32_t a32 = a;
    return (uint8_t)((s32 * a32 + d32 * (255u - a32) + 127u) / 255u);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
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
        // Process pixels in chunks to improve cache locality and allow compiler vectorization
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint8_t a = src[idx + 3];
            
            // Manual unrolling and localizing variables to help the compiler
            uint8_t s0 = src[idx];
            uint8_t s1 = src[idx + 1];
            uint8_t s2 = src[idx + 2];
            
            uint8_t d0 = dst[idx];
            uint8_t d1 = dst[idx + 1];
            uint8_t d2 = dst[idx + 2];

            // Exact calculation as required
            out[idx]     = (uint8_t)(((uint32_t)s0 * a + (uint32_t)d0 * (255u - a) + 127u) / 255u);
            out[idx + 1] = (uint8_t)(((uint32_t)s1 * a + (uint32_t)d1 * (255u - a) + 127u) / 255u);
            out[idx + 2] = (uint8_t)(((uint32_t)s2 * a + (uint32_t)d2 * (255u - a) + 127u) / 255u);
            out[idx + 3] = 255u;
        }
        
        // FNV-1a hash
        hash = 1469598103934665603ULL;
        const uint64_t prime = 1099511628211ULL;
        for (size_t i = 0; i < total_bytes; ++i) {
            hash ^= (uint64_t)out[i];
            hash *= prime;
        }
    }

    free(out);
    return hash;
}