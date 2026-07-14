#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4;
            uint8_t a = src[idx + 3];
            uint8_t inv_a = 255 - a;

            uint32_t r = ((uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127) / 255;
            uint32_t g = ((uint32_t)src[idx+1] * a + (uint32_t)dst[idx+1] * inv_a + 127) / 255;
            uint32_t b = ((uint32_t)src[idx+2] * a + (uint32_t)dst[idx+2] * inv_a + 127) / 255;

            h ^= r; h *= FNV_PRIME;
            h ^= g; h *= FNV_PRIME;
            h ^= b; h *= FNV_PRIME;
            h ^= 255; h *= FNV_PRIME;
        }
        hash = h;
    }
    return hash;
}