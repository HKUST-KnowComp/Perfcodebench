#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

// Optimized blend using the fast integer approximation for division by 255:
// (x + 1 + (x >> 8)) >> 8 is a common trick, but for the specific formula
// (s*a + d*(255-a) + 127) / 255, we can use the property that 
// x/255 ≈ (x + 1 + (x >> 8)) >> 8. 
// However, to ensure exact correctness with the original (x + 127) / 255,
// we use the identity: (val + 127) / 255 ≈ (val + 127 + (val + 127) >> 8) >> 8.
// Actually, the most robust way to match the exact integer division is to use 
// the bitwise approximation: (x + 1 + (x >> 8)) >> 8 for x/255.
// Let's use the direct math to ensure the checksum matches exactly.

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a) + 127u;
    return (uint8_t)(val / 255u);
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
    (void)width;
    (void)height;
    size_t num_bytes = (size_t)width * (size_t)height * 4u;
    uint64_t hash = 0;
    
    uint8_t* out = (uint8_t*)malloc(num_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < num_bytes; i += 4) {
            uint32_t a = src[i + 3];
            uint32_t inv_a = 255u - a;
            
            // Unrolling and using local variables to help compiler vectorize
            uint32_t s0 = src[i];
            uint32_t d0 = dst[i];
            out[i] = (uint8_t)((s0 * a + d0 * inv_a + 127u) / 255u);

            uint32_t s1 = src[i + 1];
            uint32_t d1 = dst[i + 1];
            out[i + 1] = (uint8_t)((s1 * a + d1 * inv_a + 127u) / 255u);

            uint32_t s2 = src[i + 2];
            uint32_t d2 = dst[i + 2];
            out[i + 2] = (uint8_t)((s2 * a + d2 * inv_a + 127u) / 255u);

            out[i + 3] = 255u;
        }
        hash = checksum_bytes(out, num_bytes);
    }

    free(out);
    return hash;
}