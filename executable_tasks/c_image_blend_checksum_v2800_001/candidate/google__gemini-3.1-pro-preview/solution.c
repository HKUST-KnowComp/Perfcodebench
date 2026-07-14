#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Optimized blend function using fixed-point arithmetic to replace division by 255.
 * (s * a + d * (255 - a) + 127) / 255 is exactly equal to:
 * temp = s * a + d * (255 - a) + 128
 * result = (temp + (temp >> 8)) >> 8
 */
static inline uint8_t blend_fast(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t t = s * a + d * (255u - a) + 0x80u;
    return (uint8_t)((t + (t >> 8)) >> 8);
}

/**
 * FNV-1a constants
 */
#define FNV_OFFSET_BASIS 1469598103934665603ULL
#define FNV_PRIME 1099511628211ULL

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4;
    uint64_t hash = 0;

    // Allocate buffer once outside the iteration loop to avoid malloc/free overhead
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Process pixels in a single pass to maximize cache locality
        // We blend and then checksum. 
        // Note: Compilers can often vectorize this loop if written cleanly.
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i << 2;
            uint32_t a = src[idx + 3];
            
            // Blend RGB channels
            out[idx]     = blend_fast(src[idx],     dst[idx],     a);
            out[idx + 1] = blend_fast(src[idx + 1], dst[idx + 1], a);
            out[idx + 2] = blend_fast(src[idx + 2], dst[idx + 2], a);
            // Alpha is always 255
            out[idx + 3] = 255u;
        }

        // Compute FNV-1a checksum
        uint64_t h = FNV_OFFSET_BASIS;
        const uint8_t* p = out;
        size_t n = total_bytes;
        
        // Unroll checksum loop slightly for performance
        while (n >= 4) {
            h ^= (uint64_t)p[0]; h *= FNV_PRIME;
            h ^= (uint64_t)p[1]; h *= FNV_PRIME;
            h ^= (uint64_t)p[2]; h *= FNV_PRIME;
            h ^= (uint64_t)p[3]; h *= FNV_PRIME;
            p += 4;
            n -= 4;
        }
        while (n > 0) {
            h ^= (uint64_t)*p++;
            h *= FNV_PRIME;
            n--;
        }
        hash = h;
    }

    free(out);
    return hash;
}