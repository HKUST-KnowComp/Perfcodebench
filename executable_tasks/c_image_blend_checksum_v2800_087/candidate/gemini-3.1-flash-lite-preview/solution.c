#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
    return (uint8_t)((((uint32_t)s * a) + ((uint32_t)d * (255 - a)) + 128) >> 8);
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
    size_t bytes = pixels * 4;
    uint8_t* out = (uint8_t*)malloc(bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4;
            uint8_t a = src[idx + 3];
            uint8_t inv_a = 255 - a;
            out[idx]     = (uint8_t)((((uint32_t)src[idx]     * a) + ((uint32_t)dst[idx]     * inv_a) + 128) >> 8);
            out[idx + 1] = (uint8_t)((((uint32_t)src[idx + 1] * a) + ((uint32_t)dst[idx + 1] * inv_a) + 128) >> 8);
            out[idx + 2] = (uint8_t)((((uint32_t)src[idx + 2] * a) + ((uint32_t)dst[idx + 2] * inv_a) + 128) >> 8);
            out[idx + 3] = 255;
        }
        hash = checksum_bytes(out, bytes);
    }
    free(out);
    return hash;
}