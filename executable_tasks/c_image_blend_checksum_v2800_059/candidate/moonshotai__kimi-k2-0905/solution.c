#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t byte_count = pixels * 4;
    uint8_t* out = (uint8_t*)malloc(byte_count);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s = src;
        const uint8_t* d = dst;
        uint8_t* o = out;
        size_t i = 0;

        /* process 4 pixels per iteration */
        for (; i + 3 < pixels; i += 4) {
            /* pixel 0 */
            uint32_t a0 = s[3];
            o[0] = (uint8_t)((s[0] * a0 + d[0] * (255u - a0) + 127u) / 255u);
            o[1] = (uint8_t)((s[1] * a0 + d[1] * (255u - a0) + 127u) / 255u);
            o[2] = (uint8_t)((s[2] * a0 + d[2] * (255u - a0) + 127u) / 255u);
            o[3] = 255u;

            /* pixel 1 */
            uint32_t a1 = s[7];
            o[4] = (uint8_t)((s[4] * a1 + d[4] * (255u - a1) + 127u) / 255u);
            o[5] = (uint8_t)((s[5] * a1 + d[5] * (255u - a1) + 127u) / 255u);
            o[6] = (uint8_t)((s[6] * a1 + d[6] * (255u - a1) + 127u) / 255u);
            o[7] = 255u;

            /* pixel 2 */
            uint32_t a2 = s[11];
            o[8]  = (uint8_t)((s[8]  * a2 + d[8]  * (255u - a2) + 127u) / 255u);
            o[9]  = (uint8_t)((s[9]  * a2 + d[9]  * (255u - a2) + 127u) / 255u);
            o[10] = (uint8_t)((s[10] * a2 + d[10] * (255u - a2) + 127u) / 255u);
            o[11] = 255u;

            /* pixel 3 */
            uint32_t a3 = s[15];
            o[12] = (uint8_t)((s[12] * a3 + d[12] * (255u - a3) + 127u) / 255u);
            o[13] = (uint8_t)((s[13] * a3 + d[13] * (255u - a3) + 127u) / 255u);
            o[14] = (uint8_t)((s[14] * a3 + d[14] * (255u - a3) + 127u) / 255u);
            o[15] = 255u;

            s += 16;
            d += 16;
            o += 16;
        }

        /* tail pixels */
        for (; i < pixels; ++i) {
            uint32_t a = s[3];
            o[0] = (uint8_t)((s[0] * a + d[0] * (255u - a) + 127u) / 255u);
            o[1] = (uint8_t)((s[1] * a + d[1] * (255u - a) + 127u) / 255u);
            o[2] = (uint8_t)((s[2] * a + d[2] * (255u - a) + 127u) / 255u);
            o[3] = 255u;
            s += 4;
            d += 4;
            o += 4;
        }

        hash = checksum_bytes(out, byte_count);
    }

    free(out);
    return hash;
}