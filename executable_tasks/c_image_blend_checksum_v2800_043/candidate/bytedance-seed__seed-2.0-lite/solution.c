#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a, uint8_t inv_a) {
    uint32_t x = (uint32_t)s * a + (uint32_t)d * inv_a + 127u;
    return (uint8_t)((x * 0x101u) >> 16);
}

static inline uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t out_size = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(out_size);
    if (!out) return 0;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            uint8_t inv_a = 255u - a;
            out[idx] = blend_byte(src[idx], dst[idx], a, inv_a);
            out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a, inv_a);
            out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a, inv_a);
            out[idx + 3u] = 255u;
        }
        hash = checksum_bytes(out, out_size);
    }
    free(out);
    return hash;
}