#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static uint8_t* g_buf = NULL;
static size_t   g_cap = 0;

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a, uint32_t inv_a) {
    return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * inv_a + 127u) / 255u);
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
    size_t byte_cnt = pixels * 4u;

    if (g_cap < byte_cnt) {
        free(g_buf);
        g_buf = (uint8_t*)malloc(byte_cnt);
        if (!g_buf) return 0;
        g_cap = byte_cnt;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        size_t simd_limit = pixels & ~(size_t)3;
        for (; i < simd_limit; i += 4) {
            size_t idx0 = i * 4u;
            uint8_t a0 = src[idx0 + 3];
            uint32_t inv0 = 255u - a0;
            g_buf[idx0]     = blend_byte(src[idx0],     dst[idx0],     a0, inv0);
            g_buf[idx0 + 1] = blend_byte(src[idx0 + 1], dst[idx0 + 1], a0, inv0);
            g_buf[idx0 + 2] = blend_byte(src[idx0 + 2], dst[idx0 + 2], a0, inv0);
            g_buf[idx0 + 3] = 255u;

            size_t idx1 = idx0 + 4;
            uint8_t a1 = src[idx1 + 3];
            uint32_t inv1 = 255u - a1;
            g_buf[idx1]     = blend_byte(src[idx1],     dst[idx1],     a1, inv1);
            g_buf[idx1 + 1] = blend_byte(src[idx1 + 1], dst[idx1 + 1], a1, inv1);
            g_buf[idx1 + 2] = blend_byte(src[idx1 + 2], dst[idx1 + 2], a1, inv1);
            g_buf[idx1 + 3] = 255u;

            size_t idx2 = idx1 + 4;
            uint8_t a2 = src[idx2 + 3];
            uint32_t inv2 = 255u - a2;
            g_buf[idx2]     = blend_byte(src[idx2],     dst[idx2],     a2, inv2);
            g_buf[idx2 + 1] = blend_byte(src[idx2 + 1], dst[idx2 + 1], a2, inv2);
            g_buf[idx2 + 2] = blend_byte(src[idx2 + 2], dst[idx2 + 2], a2, inv2);
            g_buf[idx2 + 3] = 255u;

            size_t idx3 = idx2 + 4;
            uint8_t a3 = src[idx3 + 3];
            uint32_t inv3 = 255u - a3;
            g_buf[idx3]     = blend_byte(src[idx3],     dst[idx3],     a3, inv3);
            g_buf[idx3 + 1] = blend_byte(src[idx3 + 1], dst[idx3 + 1], a3, inv3);
            g_buf[idx3 + 2] = blend_byte(src[idx3 + 2], dst[idx3 + 2], a3, inv3);
            g_buf[idx3 + 3] = 255u;
        }
        for (; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3];
            uint32_t inv = 255u - a;
            g_buf[idx]     = blend_byte(src[idx],     dst[idx],     a, inv);
            g_buf[idx + 1] = blend_byte(src[idx + 1], dst[idx + 1], a, inv);
            g_buf[idx + 2] = blend_byte(src[idx + 2], dst[idx + 2], a, inv);
            g_buf[idx + 3] = 255u;
        }
        hash = checksum_bytes(g_buf, byte_cnt);
    }
    return hash;
}