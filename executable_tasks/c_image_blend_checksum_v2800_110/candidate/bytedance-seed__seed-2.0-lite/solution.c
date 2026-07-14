#include "interface.h"
#include <stdint.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t final_hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET;
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            uint32_t a_src = a;
            uint32_t a_dst = 255u - a;

            // Blend R channel
            uint32_t r = (uint32_t)src[idx] * a_src + (uint32_t)dst[idx] * a_dst + 127u;
            uint8_t out_r = (uint8_t)((r * 0x8081U) >> 23);
            // Blend G channel
            uint32_t g = (uint32_t)src[idx+1] * a_src + (uint32_t)dst[idx+1] * a_dst + 127u;
            uint8_t out_g = (uint8_t)((g * 0x8081U) >> 23);
            // Blend B channel
            uint32_t b = (uint32_t)src[idx+2] * a_src + (uint32_t)dst[idx+2] * a_dst + 127u;
            uint8_t out_b = (uint8_t)((b * 0x8081U) >> 23);
            const uint8_t out_a = 255u;

            // Update FNV hash in exact original order
            hash ^= out_r; hash *= FNV_PRIME;
            hash ^= out_g; hash *= FNV_PRIME;
            hash ^= out_b; hash *= FNV_PRIME;
            hash ^= out_a; hash *= FNV_PRIME;
        }
        final_hash = hash;
    }
    return final_hash;
}