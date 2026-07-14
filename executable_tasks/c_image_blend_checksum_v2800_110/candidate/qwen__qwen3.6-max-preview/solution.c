#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    if (iters <= 0) return 0;
    
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    const uint8_t* s = src;
    const uint8_t* d = dst;
    
    for (size_t i = 0; i < pixels; ++i) {
        uint32_t a = s[3];
        uint32_t inv_a = 255u - a;
        
        uint32_t r = (s[0] * a + d[0] * inv_a + 127u) / 255u;
        uint32_t g = (s[1] * a + d[1] * inv_a + 127u) / 255u;
        uint32_t b = (s[2] * a + d[2] * inv_a + 127u) / 255u;
        
        h ^= r; h *= prime;
        h ^= g; h *= prime;
        h ^= b; h *= prime;
        h ^= 255u; h *= prime;
        
        s += 4;
        d += 4;
    }
    
    return h;
}