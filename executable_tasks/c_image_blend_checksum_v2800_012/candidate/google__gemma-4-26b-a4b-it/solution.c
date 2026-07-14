#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/* 
 * The original formula: (s*a + d*(255-a) + 127) / 255
 * This can be rewritten as: d + (s - d) * a / 255 + (127/255 if s-d > 0 else ...)
 * A common high-performance approximation for (x / 255) is ((x * 0x8081) >> 16) or similar.
 * However, to ensure EXACT correctness as per the requirement, we must match the 
 * original integer math: (s*a + d*(255-a) + 127) / 255.
 * 
 * To optimize, we use the identity: (s*a + d*(255-a) + 127) / 255 
 * = (s*a + 255*d - d*a + 127) / 255 
 * = d + (a*(s - d) + 127) / 255
 */

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t num_bytes = (size_t)width * (size_t)height * 4u;
    uint8_t* out = (uint8_t*)malloc(num_bytes);
    if (!out) return 0;

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Blending loop
        for (size_t i = 0; i < num_bytes; i += 4) {
            uint32_t s_r = src[i];
            uint32_t d_r = dst[i];
            uint32_t s_g = src[i+1];
            uint32_t d_g = dst[i+1];
            uint32_t s_b = src[i+2];
            uint32_t d_b = dst[i+2];
            uint32_t a   = src[i+3];

            // Exact math implementation of: (s*a + d*(255-a) + 127) / 255
            // Using the simplified form: d + (a*(s - d) + 127) / 255
            // We must handle the signedness of (s-d) carefully for the division.
            
            // Redefining to avoid signed issues and match original exactly:
            // res = (s*a + d*255 - d*a + 127) / 255
            // res = d + (a*(s - d) + 127) / 255
            
            // Since we need EXACT correctness, we use the original formula logic 
            // but minimize operations.
            
            uint32_t inv_a = 255u - a;
            out[i]     = (uint8_t)((s_r * a + d_r * inv_a + 127u) / 255u);
            out[i + 1] = (uint8_t)((s_g * a + d_g * inv_a + 127u) / 255u);
            out[i + 2] = (uint8_t)((s_b * a + d_b * inv_a + 127u) / 255u);
            out[i + 3] = 255u;
        }

        // Checksum calculation
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < num_bytes; ++i) {
            hash ^= (uint64_t)out[i];
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    free(out);
    return final_hash;
}