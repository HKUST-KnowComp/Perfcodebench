#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * The original blend_byte formula: ((s * a) + (d * (255 - a)) + 127) / 255
 * This is a standard linear interpolation. 
 * To optimize, we can avoid the division by 255 using the property:
 * (x + 127) / 255 is approximately (x + 127) * 0x01010101 >> 24
 * However, for exact correctness, we must ensure the result matches exactly.
 * 
 * The most significant bottleneck is the malloc/free inside the loop and the separate checksum pass.
 * We can compute the checksum on-the-fly to avoid the output buffer entirely.
 */

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;

    // Pre-calculate constants for the checksum
    const uint64_t seed = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = seed;
        
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint32_t a = src[idx + 3u];
            uint32_t inv_a = 255u - a;

            // Process R, G, B
            for (int c = 0; c < 3; ++c) {
                uint32_t s = src[idx + c];
                uint32_t d = dst[idx + c];
                uint8_t blended = (uint8_t)(((s * a) + (d * inv_a) + 127u) / 255u);
                current_hash ^= (uint64_t)blended;
                current_hash *= multiplier;
            }
            
            // Process Alpha (always 255)
            current_hash ^= (uint64_t)255u;
            current_hash *= multiplier;
        }
        hash = current_hash;
    }
    
    return hash;
}