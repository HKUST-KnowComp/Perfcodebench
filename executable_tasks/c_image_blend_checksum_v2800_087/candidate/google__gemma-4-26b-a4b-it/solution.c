#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/* 
 * The original formula: (s * a + d * (255 - a) + 127) / 255
 * To avoid division, we use the identity: x / 255 ≈ (x * 257) >> 16
 * This is a common fast approximation for 8-bit division.
 */
static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)((val * 257u) >> 16);
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
    size_t pixels = (size_t)width * (size_t)height;
    size_t bytes = pixels * 4u;
    uint64_t hash = 0;

    uint8_t* out = (uint8_t*)malloc(bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint8_t a = src[idx + 3u];
            
            // Manual unrolling and using the fast approximation
            // Note: The original code used (s*a + d*(255-a) + 127)/255.
            // To ensure EXACT correctness as per the prompt, we must use the exact math.
            // However, the prompt asks to optimize while preserving correctness.
            // Let's use the exact integer division but optimize the loop structure.
            
            uint32_t sa = (uint32_t)src[idx] * a;
            uint32_t da = (uint32_t)dst[idx] * (255u - a);
            out[idx] = (uint8_t)((sa + da + 127u) / 255u);

            sa = (uint32_t)src[idx + 1u] * a;
            da = (uint32_t)dst[idx + 1u] * (255u - a);
            out[idx + 1u] = (uint8_t)((sa + da + 127u) / 255u);

            sa = (uint32_t)src[idx + 2u] * a;
            da = (uint32_t)dst[idx + 2u] * (255u - a);
            out[idx + 2u] = (uint8_t)((sa + da + 127u) / 255u);

            out[idx + 3u] = 255u;
        }
        hash = checksum_bytes(out, bytes);
    }

    free(out);
    return hash;
}