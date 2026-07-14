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
    uint8_t* out = (uint8_t*)malloc(pixels * 4);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s = src;
        const uint8_t* d = dst;
        uint8_t* o = out;
        size_t remaining = pixels;

        while (remaining >= 4) {
            uint32_t a0 = s[3],  a1 = s[7],  a2 = s[11], a3 = s[15];
            uint32_t inv0 = 255 - a0, inv1 = 255 - a1, inv2 = 255 - a2, inv3 = 255 - a3;

            o[0]  = (uint8_t)((s[0]  * a0 + d[0]  * inv0 + 127) / 255);
            o[1]  = (uint8_t)((s[1]  * a0 + d[1]  * inv0 + 127) / 255);
            o[2]  = (uint8_t)((s[2]  * a0 + d[2]  * inv0 + 127) / 255);
            o[3]  = 255;

            o[4]  = (uint8_t)((s[4]  * a1 + d[4]  * inv1 + 127) / 255);
            o[5]  = (uint8_t)((s[5]  * a1 + d[5]  * inv1 + 127) / 255);
            o[6]  = (uint8_t)((s[6]  * a1 + d[6]  * inv1 + 127) / 255);
            o[7]  = 255;

            o[8]  = (uint8_t)((s[8]  * a2 + d[8]  * inv2 + 127) / 255);
            o[9]  = (uint8_t)((s[9]  * a2 + d[9]  * inv2 + 127) / 255);
            o[10] = (uint8_t)((s[10] * a2 + d[10] * inv2 + 127) / 255);
            o[11] = 255;

            o[12] = (uint8_t)((s[12] * a3 + d[12] * inv3 + 127) / 255);
            o[13] = (uint8_t)((s[13] * a3 + d[13] * inv3 + 127) / 255);
            o[14] = (uint8_t)((s[14] * a3 + d[14] * inv3 + 127) / 255);
            o[15] = 255;

            s += 16; d += 16; o += 16;
            remaining -= 4;
        }

        while (remaining) {
            uint32_t a = s[3];
            uint32_t inv = 255 - a;
            o[0] = (uint8_t)((s[0] * a + d[0] * inv + 127) / 255);
            o[1] = (uint8_t)((s[1] * a + d[1] * inv + 127) / 255);
            o[2] = (uint8_t)((s[2] * a + d[2] * inv + 127) / 255);
            o[3] = 255;
            s += 4; d += 4; o += 4;
            --remaining;
        }

        hash = checksum_bytes(out, pixels * 4);
    }
    free(out);
    return hash;
}