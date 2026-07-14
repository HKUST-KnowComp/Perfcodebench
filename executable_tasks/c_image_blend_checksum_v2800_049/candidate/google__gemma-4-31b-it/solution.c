#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Fast division by 255 using fixed-point multiplication.
 * (x + 127) / 255 is approximately ((x + 127) * 0x8081) >> 23
 */
static inline uint8_t fast_blend(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = (uint32_t)s * a + (uint32_t)d * (255u - a);
    return (uint8_t)(((val + 127u) * 0x8081u) >> 23);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    // Process in 8-byte chunks to reduce loop overhead
    size_t i = 0;
    while (i + 8 <= n) {
        for (int j = 0; j < 8; ++j) {
            hash ^= (uint64_t)data[i++];
            hash *= prime;
        }
    }
    while (i < n) {
        hash ^= (uint64_t)data[i++];
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
            o_ptr[0] = fast_blend(s_ptr[0], d_ptr[0], a);
            o_ptr[1] = fast_blend(s_ptr[1], d_ptr[1], a);
            o_ptr[2] = fast_blend(s_ptr[2], d_ptr[2], a);
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