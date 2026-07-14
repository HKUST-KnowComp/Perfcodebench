#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4;
            uint8_t a = src[idx + 3];
            uint32_t ia = 255u - a;

            uint32_t r = ((uint32_t)src[idx] * a + (uint32_t)dst[idx] * ia + 127u) / 255u;
            uint32_t g = ((uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * ia + 127u) / 255u;
            uint32_t b = ((uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * ia + 127u) / 255u;

            hash ^= (uint64_t)r; hash *= FNV_PRIME;
            hash ^= (uint64_t)g; hash *= FNV_PRIME;
            hash ^= (uint64_t)b; hash *= FNV_PRIME;
            hash ^= 255ULL; hash *= FNV_PRIME;
        }
    }
    return hash;
}