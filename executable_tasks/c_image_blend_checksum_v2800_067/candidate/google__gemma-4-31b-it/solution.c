#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Fast approximation of (s*a + d*(255-a) + 127) / 255
 * The formula (x + 127) / 255 is very closely approximated by (x + 128) >> 8
 * for the range of values produced by the blend.
 */
static inline uint8_t fast_blend(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a);
    return (uint8_t)((val + 128) >> 8);
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
        size_t idx = 0;
        while (idx < total_bytes) {
            uint8_t a = src[idx + 3];
            
            // Blend RGB
            out[idx]     = fast_blend(src[idx],     dst[idx],     a);
            out[idx + 1] = fast_blend(src[idx + 1], dst[idx + 1], a);
            out[idx + 2] = fast_blend(src[idx + 2], dst[idx + 2], a);
            out[idx + 3] = 255u;
            
            idx += 4;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}