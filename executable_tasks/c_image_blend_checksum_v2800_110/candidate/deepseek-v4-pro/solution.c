#include "interface.h"
#include <stdint.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t HSTART = 1469598103934665603ULL;
    const uint64_t HMUL  = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = HSTART;
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint32_t a32 = (uint32_t)src[idx + 3u];
            uint32_t inv_a = 255u - a32;

            // red
            uint32_t s0 = (uint32_t)src[idx];
            uint32_t d0 = (uint32_t)dst[idx];
            uint8_t r = (uint8_t)((s0 * a32 + d0 * inv_a + 127u) / 255u);
            h ^= r; h *= HMUL;

            // green
            uint32_t s1 = (uint32_t)src[idx + 1u];
            uint32_t d1 = (uint32_t)dst[idx + 1u];
            uint8_t g = (uint8_t)((s1 * a32 + d1 * inv_a + 127u) / 255u);
            h ^= g; h *= HMUL;

            // blue
            uint32_t s2 = (uint32_t)src[idx + 2u];
            uint32_t d2 = (uint32_t)dst[idx + 2u];
            uint8_t b = (uint8_t)((s2 * a32 + d2 * inv_a + 127u) / 255u);
            h ^= b; h *= HMUL;

            // alpha (always 255)
            h ^= 255u; h *= HMUL;
        }
        hash = h;
    }
    return hash;
}