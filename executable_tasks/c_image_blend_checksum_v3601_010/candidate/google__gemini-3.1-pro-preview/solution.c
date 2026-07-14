#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Bit-exact replacement for (v + 127) / 255 for v in [0, 255*255]
// Formula: (v + 128 + ((v + 128) >> 8)) >> 8
static inline uint8_t blend_byte_fast(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t v = s * a + d * (255u - a) + 128u;
    return (uint8_t)((v + (v >> 8u)) >> 8u);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    size_t i = 0;
    // Unroll loop to improve ILP
    for (; i + 3 < n; i += 4) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
        hash ^= (uint64_t)data[i + 1];
        hash *= prime;
        hash ^= (uint64_t)data[i + 2];
        hash *= prime;
        hash ^= (uint64_t)data[i + 3];
        hash *= prime;
    }
    for (; i < n; ++i) {
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
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint32_t a = src[idx + 3];
            
            // Alpha blending components
            out[idx]     = blend_byte_fast(src[idx],     dst[idx],     a);
            out[idx + 1] = blend_byte_fast(src[idx + 1], dst[idx + 1], a);
            out[idx + 2] = blend_byte_fast(src[idx + 2], dst[idx + 2], a);
            out[idx + 3] = 255u;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}