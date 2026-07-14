#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const uint8_t* s = src;
        const uint8_t* d = dst;
        for (size_t i = 0; i < pixels; ++i) {
            uint32_t a = s[3];
            uint32_t inv_a = 255 - a;

            uint32_t r = (s[0] * a + d[0] * inv_a + 127) / 255;
            uint32_t g = (s[1] * a + d[1] * inv_a + 127) / 255;
            uint32_t b = (s[2] * a + d[2] * inv_a + 127) / 255;

            hash ^= r; hash *= fnv_prime;
            hash ^= g; hash *= fnv_prime;
            hash ^= b; hash *= fnv_prime;
            hash ^= 255; hash *= fnv_prime;

            s += 4;
            d += 4;
        }
    }
    return hash;
}
