#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    if (iters <= 0) return 0;
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t i = 0; i < pixels; ++i) {
        size_t idx = i * 4u;
        uint32_t a = src[idx + 3u];
        uint32_t da = 255u - a;
        
        uint32_t v0 = (uint32_t)src[idx] * a + (uint32_t)dst[idx] * da + 127u;
        uint32_t v1 = (uint32_t)src[idx+1] * a + (uint32_t)dst[idx+1] * da + 127u;
        uint32_t v2 = (uint32_t)src[idx+2] * a + (uint32_t)dst[idx+2] * da + 127u;
        
        uint8_t b0 = (uint8_t)((v0 * 257u + 32895u) >> 16);
        uint8_t b1 = (uint8_t)((v1 * 257u + 32895u) >> 16);
        uint8_t b2 = (uint8_t)((v2 * 257u + 32895u) >> 16);
        uint8_t b3 = 255u;
        
        hash ^= b0; hash *= 1099511628211ULL;
        hash ^= b1; hash *= 1099511628211ULL;
        hash ^= b2; hash *= 1099511628211ULL;
        hash ^= b3; hash *= 1099511628211ULL;
    }
    return hash;
}
