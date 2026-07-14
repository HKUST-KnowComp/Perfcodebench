#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t checksum_bytes(const uint8_t* restrict data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint8_t* restrict src, const uint8_t* restrict dst, int width, int height, int iters) {
    const size_t pixels = (size_t)width * (size_t)height;
    const size_t out_size = pixels * 4U;
    uint8_t* restrict out = (uint8_t*)malloc(out_size);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < pixels; ++i) {
            const size_t idx = i * 4U;
            const uint32_t a = src[idx + 3U];
            const uint32_t inv_a = 255U - a;

            // Blend red channel
            const uint32_t s_r = src[idx];
            const uint32_t d_r = dst[idx];
            out[idx] = (uint8_t)((s_r * a + d_r * inv_a + 127U) / 255U);

            // Blend green channel
            const uint32_t s_g = src[idx + 1U];
            const uint32_t d_g = dst[idx + 1U];
            out[idx + 1U] = (uint8_t)((s_g * a + d_g * inv_a + 127U) / 255U);

            // Blend blue channel
            const uint32_t s_b = src[idx + 2U];
            const uint32_t d_b = dst[idx + 2U];
            out[idx + 2U] = (uint8_t)((s_b * a + d_b * inv_a + 127U) / 255U);

            // Set output alpha to fully opaque
            out[idx + 3U] = 255U;
        }
        hash = checksum_bytes(out, out_size);
    }

    free(out);
    return hash;
}