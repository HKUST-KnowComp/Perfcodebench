#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte(uint32_t s, uint32_t d, uint32_t a, uint32_t inv_a) {
    uint32_t num = s * a + d * inv_a + 127u;
    return (uint8_t)((num * 0x8081U) >> 23);
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
    uint8_t* __restrict__ out = (uint8_t*)malloc(pixels * 4u);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            uint32_t inv_a = 255u - a;
            out[idx] = blend_byte(src[idx], dst[idx], a, inv_a);
            out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a, inv_a);
            out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a, inv_a);
            out[idx + 3u] = 255u;
        }
        hash = checksum_bytes(out, pixels * 4u);
    }

    free(out);
    return hash;
}