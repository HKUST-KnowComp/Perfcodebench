#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Optimized blend using the property: (s*a + d*(255-a) + 127) / 255 
// can be approximated very closely or exactly in 8-bit space using bit shifts.
// A common fast approximation for (x + 127) / 255 is (x + 1 + (x >> 8)) >> 8.
static inline uint8_t blend_byte_fast(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t t = s * a + d * (255u - a) + 127u;
    return (uint8_t)((t + (t >> 8)) >> 8);
}

// FNV-1a hash implementation
static inline uint64_t checksum_bytes(const uint8_t* data, size_t n) {
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
            uint32_t a = s_ptr[3];
            // Unroll channels
            uint32_t s0 = s_ptr[0], d0 = d_ptr[0];
            uint32_t s1 = s_ptr[1], d1 = d_ptr[1];
            uint32_t s2 = s_ptr[2], d2 = d_ptr[2];

            // Standard blend: (s*a + d*(255-a) + 127) / 255
            // Using the exact integer division logic provided in baseline
            o_ptr[0] = (uint8_t)((s0 * a + d0 * (255u - a) + 127u) / 255u);
            o_ptr[1] = (uint8_t)((s1 * a + d1 * (255u - a) + 127u) / 255u);
            o_ptr[2] = (uint8_t)((s2 * a + d2 * (255u - a) + 127u) / 255u);
            o_ptr[3] = 255u;

            s_ptr += 4;
            d_ptr += 4;
            o_ptr += 4;
        }
        
        // FNV-1a hash
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_bytes; ++i) {
            hash ^= (uint64_t)out[i];
            hash *= 1099511628211ULL;
        }
    }

    free(out);
    return hash;
}