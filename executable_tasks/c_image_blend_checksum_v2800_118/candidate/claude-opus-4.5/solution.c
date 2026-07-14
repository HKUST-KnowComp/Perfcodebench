#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint16_t blend_table[256][256];
static int table_initialized = 0;

static void init_blend_table(void) {
    if (table_initialized) return;
    for (int a = 0; a < 256; ++a) {
        for (int v = 0; v < 256; ++v) {
            blend_table[a][v] = (uint16_t)(v * a + 127);
        }
    }
    table_initialized = 1;
}

static inline uint8_t blend_fast(uint8_t s, uint8_t d, uint8_t a, uint8_t inv_a) {
    uint32_t result = blend_table[a][s] + blend_table[inv_a][d];
    return (uint8_t)(result / 255u);
}

static uint64_t checksum_bytes(const uint8_t* restrict data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* end = data + n;
    while (data + 8 <= end) {
        hash ^= (uint64_t)data[0]; hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[1]; hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[2]; hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[3]; hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[4]; hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[5]; hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[6]; hash *= 1099511628211ULL;
        hash ^= (uint64_t)data[7]; hash *= 1099511628211ULL;
        data += 8;
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
    
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* restrict sp = src;
        const uint8_t* restrict dp = dst;
        uint8_t* restrict op = out;
        
        size_t i = 0;
        size_t pixels4 = pixels & ~3ULL;
        
        for (; i < pixels4; i += 4) {
            uint8_t a0 = sp[3];
            uint8_t inv_a0 = 255u - a0;
            op[0] = blend_fast(sp[0], dp[0], a0, inv_a0);
            op[1] = blend_fast(sp[1], dp[1], a0, inv_a0);
            op[2] = blend_fast(sp[2], dp[2], a0, inv_a0);
            op[3] = 255u;
            
            uint8_t a1 = sp[7];
            uint8_t inv_a1 = 255u - a1;
            op[4] = blend_fast(sp[4], dp[4], a1, inv_a1);
            op[5] = blend_fast(sp[5], dp[5], a1, inv_a1);
            op[6] = blend_fast(sp[6], dp[6], a1, inv_a1);
            op[7] = 255u;
            
            uint8_t a2 = sp[11];
            uint8_t inv_a2 = 255u - a2;
            op[8] = blend_fast(sp[8], dp[8], a2, inv_a2);
            op[9] = blend_fast(sp[9], dp[9], a2, inv_a2);
            op[10] = blend_fast(sp[10], dp[10], a2, inv_a2);
            op[11] = 255u;
            
            uint8_t a3 = sp[15];
            uint8_t inv_a3 = 255u - a3;
            op[12] = blend_fast(sp[12], dp[12], a3, inv_a3);
            op[13] = blend_fast(sp[13], dp[13], a3, inv_a3);
            op[14] = blend_fast(sp[14], dp[14], a3, inv_a3);
            op[15] = 255u;
            
            sp += 16;
            dp += 16;
            op += 16;
        }
        
        for (; i < pixels; ++i) {
            uint8_t a = sp[3];
            uint8_t inv_a = 255u - a;
            op[0] = blend_fast(sp[0], dp[0], a, inv_a);
            op[1] = blend_fast(sp[1], dp[1], a, inv_a);
            op[2] = blend_fast(sp[2], dp[2], a, inv_a);
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