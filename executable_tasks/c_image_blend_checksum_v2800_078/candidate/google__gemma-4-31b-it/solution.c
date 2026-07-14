#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/* 
 * The original blend_byte is: ((s * a) + (d * (255 - a)) + 127) / 255
 * This can be rewritten as: (s * a + d * 255 - d * a + 127) / 255
 * = d + (a * (s - d) + 127) / 255
 */
static inline uint8_t fast_blend(uint8_t s, uint8_t d, uint8_t a) {
    int32_t val = (int32_t)a * ((int32_t)s - (int32_t)d);
    return (uint8_t)(d + (val + 127) / 255);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3];
            
            // Optimization: if alpha is 0, result is dst. If 255, result is src.
            if (a == 0) {
                out[idx] = dst[idx];
                out[idx + 1] = dst[idx + 1];
                out[idx + 2] = dst[idx + 2];
            } else if (a == 255) {
                out[idx] = src[idx];
                out[idx + 1] = src[idx + 1];
                out[idx + 2] = src[idx + 2];
            } else {
                out[idx] = fast_blend(src[idx], dst[idx], a);
                out[idx + 1] = fast_blend(src[idx + 1], dst[idx + 1], a);
                out[idx + 2] = fast_blend(src[idx + 2], dst[idx + 2], a);
            }
            out[idx + 3] = 255;
        }

        // Inline checksum_bytes to avoid function call overhead and allow compiler optimization
        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < bytes; ++i) {
            current_hash ^= (uint64_t)out[i];
            current_hash *= 1099511628211ULL;
        }
        hash = current_hash;
    }

    free(out);
    return hash;
}