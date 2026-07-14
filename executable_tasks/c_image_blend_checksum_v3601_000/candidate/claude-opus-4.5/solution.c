#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* restrict src, const uint8_t* restrict dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;
    
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* sp = src;
        const uint8_t* dp = dst;
        uint8_t* op = out;
        
        for (size_t i = 0; i < pixels; ++i) {
            uint32_t a = sp[3];
            uint32_t inv_a = 255u - a;
            
            uint32_t r = (uint32_t)sp[0] * a + (uint32_t)dp[0] * inv_a + 127u;
            uint32_t g = (uint32_t)sp[1] * a + (uint32_t)dp[1] * inv_a + 127u;
            uint32_t b = (uint32_t)sp[2] * a + (uint32_t)dp[2] * inv_a + 127u;
            
            op[0] = (uint8_t)(((r + 128u) * 257u) >> 16);
            op[1] = (uint8_t)(((g + 128u) * 257u) >> 16);
            op[2] = (uint8_t)(((b + 128u) * 257u) >> 16);
            op[3] = 255u;
            
            sp += 4;
            dp += 4;
            op += 4;
        }
        
        uint64_t h = 1469598103934665603ULL;
        const uint8_t* data = out;
        size_t n = total_bytes;
        
        size_t chunks = n >> 3;
        for (size_t i = 0; i < chunks; ++i) {
            h ^= (uint64_t)data[0]; h *= 1099511628211ULL;
            h ^= (uint64_t)data[1]; h *= 1099511628211ULL;
            h ^= (uint64_t)data[2]; h *= 1099511628211ULL;
            h ^= (uint64_t)data[3]; h *= 1099511628211ULL;
            h ^= (uint64_t)data[4]; h *= 1099511628211ULL;
            h ^= (uint64_t)data[5]; h *= 1099511628211ULL;
            h ^= (uint64_t)data[6]; h *= 1099511628211ULL;
            h ^= (uint64_t)data[7]; h *= 1099511628211ULL;
            data += 8;
        }
        
        size_t rem = n & 7;
        for (size_t i = 0; i < rem; ++i) {
            h ^= (uint64_t)data[i];
            h *= 1099511628211ULL;
        }
        
        hash = h;
    }
    
    free(out);
    return hash;
}
