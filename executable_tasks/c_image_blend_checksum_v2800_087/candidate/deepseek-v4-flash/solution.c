#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL; // FNV offset basis
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4;
            uint32_t a = src[idx + 3];
            uint32_t inv_a = 255 - a;
            // blend each component with (s*a + d*(255-a) + 127) / 255
            uint32_t r = (src[idx] * a + dst[idx] * inv_a + 127) / 255;
            uint32_t g = (src[idx + 1] * a + dst[idx + 1] * inv_a + 127) / 255;
            uint32_t b = (src[idx + 2] * a + dst[idx + 2] * inv_a + 127) / 255;
            // update FNV-1a hash with the four bytes: R, G, B, 255
            h ^= (uint64_t)(uint8_t)r;
            h *= 1099511628211ULL;
            h ^= (uint64_t)(uint8_t)g;
            h *= 1099511628211ULL;
            h ^= (uint64_t)(uint8_t)b;
            h *= 1099511628211ULL;
            h ^= 255ULL;
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    return hash;
}
