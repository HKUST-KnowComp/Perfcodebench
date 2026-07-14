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

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4u;
    uint64_t hash = 0;
    
    init_blend_table();
    
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* restrict sp = src;
        const uint8_t* restrict dp = dst;
        uint8_t* restrict op = out;
        
        size_t i = 0;
        size_t pixels4 = pixels & ~(size_t)3;
        
        for (; i < pixels4; i += 4) {
            uint8_t a0 = sp[3];
            uint8_t a1 = sp[7];
            uint8_t a2 = sp[11];
            uint8_t a3 = sp[15];
            
            uint32_t inv_a0 = 255u - a0;
            uint32_t inv_a1 = 255u - a1;
            uint32_t inv_a2 = 255u - a2;
            uint32_t inv_a3 = 255u - a3;
            
            op[0] = blend_table[a0][sp[0]] + blend_table[inv_a0][dp[0]];
            op[1] = blend_table[a0][sp[1]] + blend_table[inv_a0][dp[1]];
            op[2] = blend_table[a0][sp[2]] + blend_table[inv_a0][dp[2]];
            op[3] = 255u;
            
            op[4] = blend_table[a1][sp[4]] + blend_table[inv_a1][dp[4]];
            op[5] = blend_table[a1][sp[5]] + blend_table[inv_a1][dp[5]];
            op[6] = blend_table[a1][sp[6]] + blend_table[inv_a1][dp[6]];
            op[7] = 255u;
            
            op[8] = blend_table[a2][sp[8]] + blend_table[inv_a2][dp[8]];
            op[9] = blend_table[a2][sp[9]] + blend_table[inv_a2][dp[9]];
            op[10] = blend_table[a2][sp[10]] + blend_table[inv_a2][dp[10]];
            op[11] = 255u;
            
            op[12] = blend_table[a3][sp[12]] + blend_table[inv_a3][dp[12]];
            op[13] = blend_table[a3][sp[13]] + blend_table[inv_a3][dp[13]];
            op[14] = blend_table[a3][sp[14]] + blend_table[inv_a3][dp[14]];
            op[15] = 255u;
            
            sp += 16;
            dp += 16;
            op += 16;
        }
        
        for (; i < pixels; ++i) {
            uint8_t a = sp[3];
            uint32_t inv_a = 255u - a;
            op[0] = blend_table[a][sp[0]] + blend_table[inv_a][dp[0]];
            op[1] = blend_table[a][sp[1]] + blend_table[inv_a][dp[1]];
            op[2] = blend_table[a][sp[2]] + blend_table[inv_a][dp[2]];
            op[3] = 255u;
            sp += 4;
            dp += 4;
            op += 4;
        }
        
        uint64_t h = 1469598103934665603ULL;
        const uint8_t* data = out;
        size_t n = total_bytes;
        size_t j = 0;
        size_t n8 = n & ~(size_t)7;
        
        for (; j < n8; j += 8) {
            h ^= (uint64_t)data[j];
            h *= 1099511628211ULL;
            h ^= (uint64_t)data[j+1];
            h *= 1099511628211ULL;
            h ^= (uint64_t)data[j+2];
            h *= 1099511628211ULL;
            h ^= (uint64_t)data[j+3];
            h *= 1099511628211ULL;
            h ^= (uint64_t)data[j+4];
            h *= 1099511628211ULL;
            h ^= (uint64_t)data[j+5];
            h *= 1099511628211ULL;
            h ^= (uint64_t)data[j+6];
            h *= 1099511628211ULL;
            h ^= (uint64_t)data[j+7];
            h *= 1099511628211ULL;
        }
        
        for (; j < n; ++j) {
            h ^= (uint64_t)data[j];
            h *= 1099511628211ULL;
        }
        
        hash = h;
    }
    
    free(out);
    return hash;
}