#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Fast approximation of (s * a + d * (255 - a) + 127) / 255
 * The formula (x + 127) / 255 can be approximated as (x + 1 + (x >> 8)) >> 8
 * for 0 <= x <= 255*255.
 * 
 * Let x = s*a + d*(255-a).
 * The exact result is floor((x + 127) / 255).
 * A known fast integer approximation for (val + 127) / 255 is ((val + 128) * 0x8081) >> 23
 * or more simply for this specific range: ((val + 1) + (val >> 8)) >> 8.
 */
static inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t x = (uint32_t)s * a + (uint32_t)d * (255u - a);
    return (uint8_t)(((x + 1) + (x >> 8)) >> 8);
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
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        for (; i < pixels; ++i) {
            size_t idx = i << 2;
            uint8_t a = src[idx + 3];
            
            // Optimization: if alpha is 0 or 255, we can skip the blend math
            if (a == 0) {
                out[idx] = dst[idx];
                out[idx + 1] = dst[idx + 1];
                out[idx + 2] = dst[idx + 2];
            } else if (a == 255) {
                out[idx] = src[idx];
                out[idx + 1] = src[idx + 1];
                out[idx + 2] = src[idx + 2];
            } else {
                out[idx] = blend_fast(src[idx], dst[idx], a);
                out[idx + 1] = blend_fast(src[idx + 1], dst[idx + 1], a);
                out[idx + 2] = blend_fast(src[idx + 2], dst[idx + 2], a);
            }
            out[idx + 3] = 255;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}