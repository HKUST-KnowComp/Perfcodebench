#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint8_t blend_table[256][256];
static int table_initialized = 0;

static void init_blend_table(void) {
    if (table_initialized) return;
    for (int a = 0; a < 256; ++a) {
        for (int s = 0; s < 256; ++s) {
            blend_table[a][s] = (uint8_t)(((uint32_t)s * (uint32_t)a + 127u) / 255u);
        }
    }
    table_initialized = 1;
}

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    return blend_table[a][s] + blend_table[255u - a][d];
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* end = data + n;
    const uint8_t* end4 = data + (n & ~(size_t)3);
    
    while (data < end4) {
        hash ^= (uint64_t)data[0];
        hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[1];
        hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[2];
        hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[3];
        hash *= 1099511628211ULL;
        data += 4;
    }
    
    while (data < end) {
        hash ^= (uint64_t)*data++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    init_blend_table();
    
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;
    
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* sp = src;
        const uint8_t* dp = dst;
        uint8_t* op = out;
        const uint8_t* sp_end = src + total_bytes;
        
        while (sp < sp_end) {
            uint8_t a = sp[3];
            uint8_t inv_a = 255u - a;
            
            op[0] = blend_table[a][sp[0]] + blend_table[inv_a][dp[0]];
            op[1] = blend_table[a][sp[1]] + blend_table[inv_a][dp[1]];
            op[2] = blend_table[a][sp[2]] + blend_table[inv_a][dp[2]];
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