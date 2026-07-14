#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Fast approximation of (s * a + d * (255 - a) + 127) / 255
 * The formula (val + 127) / 255 can be approximated as (val + 1 + (val >> 8)) >> 8
 * for 16-bit values. 
 * Specifically, (s*a + d*(255-a) + 127) / 255 is equivalent to
 * (s*a + d*255 - d*a + 127) / 255 = d + (a*(s-d) + 127) / 255.
 */
static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t s32 = s;
    uint32_t d32 = d;
    uint32_t a32 = a;
    uint32_t res = (s32 * a32 + d32 * (255u - a32) + 127u);
    // Division by 255 is approximately (x + 1 + (x >> 8)) >> 8
    return (uint8_t)((res + 1 + (res >> 8)) >> 8);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s_ptr = src;
        const uint8_t* d_ptr = dst;
        uint8_t* o_ptr = out;

        for (size_t i = 0; i < pixels; ++i) {
            uint8_t a = s_ptr[3];
            o_ptr[0] = blend_byte_fast(s_ptr[0], d_ptr[0], a);
            o_ptr[1] = blend_byte_fast(s_ptr[1], d_ptr[1], a);
            o_ptr[2] = blend_byte_fast(s_ptr[2], d_ptr[2], a);
            o_ptr[3] = 255u;
            s_ptr += 4;
            d_ptr += 4;
            o_ptr += 4;
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}