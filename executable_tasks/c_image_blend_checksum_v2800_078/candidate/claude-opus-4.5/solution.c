#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint16_t blend_table[256 * 256];
static int table_initialized = 0;

static void init_blend_table(void) {
    if (table_initialized) return;
    for (int a = 0; a < 256; ++a) {
        for (int x = 0; x < 256; ++x) {
            blend_table[a * 256 + x] = (uint16_t)((x * a + 127) / 255);
        }
    }
    table_initialized = 1;
}

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t sa = blend_table[a * 256 + s];
    uint32_t da = blend_table[(255 - a) * 256 + d];
    return (uint8_t)(sa + da);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
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
    init_blend_table();
    
    size_t pixels = (size_t)width * (size_t)height;
    size_t bytes = pixels * 4u;
    uint64_t hash = 0;
    
    uint8_t* out = (uint8_t*)malloc(bytes);
    if (!out) return 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* sp = src;
        const uint8_t* dp = dst;
        uint8_t* op = out;
        
        for (size_t i = 0; i < pixels; ++i) {
            uint8_t a = sp[3];
            uint8_t r = blend_byte_fast(sp[0], dp[0], a);
            uint8_t g = blend_byte_fast(sp[1], dp[1], a);
            uint8_t b = blend_byte_fast(sp[2], dp[2], a);
            
            op[0] = r;
            op[1] = g;
            op[2] = b;
            op[3] = 255u;
            
            sp += 4;
            dp += 4;
            op += 4;
        }
        
        hash = checksum_bytes(out, bytes);
    }
    
    free(out);
    return hash;
}