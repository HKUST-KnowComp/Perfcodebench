#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a, uint8_t inv_a) {
  return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)inv_a + 127u) / 255u);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* end = data + n;
  const uint8_t* end8 = data + (n & ~(size_t)7);
  
  while (data < end8) {
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
  size_t pixels = (size_t)width * (size_t)height;
  size_t bytes = pixels * 4u;
  uint64_t hash = 0;
  
  uint8_t* out = (uint8_t*)malloc(bytes);
  if (!out) return 0;
  
  static const uint8_t inv_table[256] = {
    255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,
    239,238,237,236,235,234,233,232,231,230,229,228,227,226,225,224,
    223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208,
    207,206,205,204,203,202,201,200,199,198,197,196,195,194,193,192,
    191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176,
    175,174,173,172,171,170,169,168,167,166,165,164,163,162,161,160,
    159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144,
    143,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128,
    127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,
    111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,
    95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,
    79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,
    63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,
    47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,
    31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,
    15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
  };
  
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* sp = src;
    const uint8_t* dp = dst;
    uint8_t* op = out;
    
    size_t i = 0;
    size_t pixels4 = pixels & ~(size_t)3;
    
    for (; i < pixels4; i += 4) {
      uint8_t a0 = sp[3];
      uint8_t inv_a0 = inv_table[a0];
      op[0] = blend_byte(sp[0], dp[0], a0, inv_a0);
      op[1] = blend_byte(sp[1], dp[1], a0, inv_a0);
      op[2] = blend_byte(sp[2], dp[2], a0, inv_a0);
      op[3] = 255u;
      
      uint8_t a1 = sp[7];
      uint8_t inv_a1 = inv_table[a1];
      op[4] = blend_byte(sp[4], dp[4], a1, inv_a1);
      op[5] = blend_byte(sp[5], dp[5], a1, inv_a1);
      op[6] = blend_byte(sp[6], dp[6], a1, inv_a1);
      op[7] = 255u;
      
      uint8_t a2 = sp[11];
      uint8_t inv_a2 = inv_table[a2];
      op[8] = blend_byte(sp[8], dp[8], a2, inv_a2);
      op[9] = blend_byte(sp[9], dp[9], a2, inv_a2);
      op[10] = blend_byte(sp[10], dp[10], a2, inv_a2);
      op[11] = 255u;
      
      uint8_t a3 = sp[15];
      uint8_t inv_a3 = inv_table[a3];
      op[12] = blend_byte(sp[12], dp[12], a3, inv_a3);
      op[13] = blend_byte(sp[13], dp[13], a3, inv_a3);
      op[14] = blend_byte(sp[14], dp[14], a3, inv_a3);
      op[15] = 255u;
      
      sp += 16;
      dp += 16;
      op += 16;
    }
    
    for (; i < pixels; ++i) {
      uint8_t a = sp[3];
      uint8_t inv_a = inv_table[a];
      op[0] = blend_byte(sp[0], dp[0], a, inv_a);
      op[1] = blend_byte(sp[1], dp[1], a, inv_a);
      op[2] = blend_byte(sp[2], dp[2], a, inv_a);
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