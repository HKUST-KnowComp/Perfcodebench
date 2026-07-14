#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline uint8_t blend_fast(uint32_t s, uint32_t d, uint32_t a) {
    uint32_t inv_a = 255u - a;
    uint32_t val = s * a + d * inv_a + 128u;
    return (uint8_t)((val + (val >> 8)) >> 8);
}

static uint64_t checksum_bytes(const uint8_t* restrict data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    size_t i = 0;
    size_t n8 = n & ~(size_t)7;
    
    for (; i < n8; i += 8) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
        hash ^= (uint64_t)data[i + 1];
        hash *= prime;
        hash ^= (uint64_t)data[i + 2];
        hash *= prime;
        hash ^= (uint64_t)data[i + 3];
        hash *= prime;
        hash ^= (uint64_t)data[i + 4];
        hash *= prime;
        hash ^= (uint64_t)data[i + 5];
        hash *= prime;
        hash ^= (uint64_t)data[i + 6];
        hash *= prime;
        hash ^= (uint64_t)data[i + 7];
        hash *= prime;
    }
    
    for (; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
    }
    
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;
    
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* restrict sp = src;
        const uint8_t* restrict dp = dst;
        uint8_t* restrict op = out;
        
        for (size_t i = 0; i < pixels; ++i) {
            uint32_t a = sp[3];
            uint32_t inv_a = 255u - a;
            
            uint32_t val0 = (uint32_t)sp[0] * a + (uint32_t)dp[0] * inv_a + 128u;
            uint32_t val1 = (uint32_t)sp[1] * a + (uint32_t)dp[1] * inv_a + 128u;
            uint32_t val2 = (uint32_t)sp[2] * a + (uint32_t)dp[2] * inv_a + 128u;
            
            op[0] = (uint8_t)((val0 + (val0 >> 8)) >> 8);
            op[1] = (uint8_t)((val1 + (val1 >> 8)) >> 8);
            op[2] = (uint8_t)((val2 + (val2 >> 8)) >> 8);
            op[3] = 255u;
            
            sp += 4;
            dp += 4;
            op += 4;
        }
        
        hash = checksum_bytes(out, total_bytes);
    }
    
    free(out);
    return hash;
}