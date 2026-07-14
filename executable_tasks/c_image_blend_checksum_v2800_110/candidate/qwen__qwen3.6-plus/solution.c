#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        const uint8_t* s = src;
        const uint8_t* d = dst;
        for (size_t i = 0; i < pixels; ++i) {
            uint8_t a = s[3];
            uint8_t ia = 255 - a;
            
            uint32_t r = s[0];
            uint32_t g = s[1];
            uint32_t b = s[2];
            uint32_t dr = d[0];
            uint32_t dg = d[1];
            uint32_t db = d[2];
            
            h ^= (uint8_t)((r * a + dr * ia + 127) / 255); h *= FNV_PRIME;
            h ^= (uint8_t)((g * a + dg * ia + 127) / 255); h *= FNV_PRIME;
            h ^= (uint8_t)((b * a + db * ia + 127) / 255); h *= FNV_PRIME;
            h ^= 255; h *= FNV_PRIME;
            
            s += 4;
            d += 4;
        }
        hash = h;
    }
    return hash;
}