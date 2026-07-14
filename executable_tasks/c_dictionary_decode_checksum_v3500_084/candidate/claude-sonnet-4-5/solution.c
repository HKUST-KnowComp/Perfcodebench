#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* restrict ids, size_t n, const uint32_t* restrict dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;
    
    size_t i = 0;
    size_t n_unroll = n & ~3ULL;
    
    uint64_t h0 = h, h1 = h, h2 = h, h3 = h;
    
    for (; i < n_unroll; i += 4) {
      uint32_t v0 = dict_vals[ids[i]];
      uint32_t v1 = dict_vals[ids[i+1]];
      uint32_t v2 = dict_vals[ids[i+2]];
      uint32_t v3 = dict_vals[ids[i+3]];
      
      h0 ^= (uint64_t)v0 + 1ULL;
      h0 *= FNV_PRIME;
      
      h1 ^= (uint64_t)v1 + 1ULL;
      h1 *= FNV_PRIME;
      
      h2 ^= (uint64_t)v2 + 1ULL;
      h2 *= FNV_PRIME;
      
      h3 ^= (uint64_t)v3 + 1ULL;
      h3 *= FNV_PRIME;
    }
    
    h = h0 ^ h1 ^ h2 ^ h3;
    
    for (; i < n; ++i) {
      uint32_t val = dict_vals[ids[i]];
      h ^= (uint64_t)val + 1ULL;
      h *= FNV_PRIME;
    }
  }
  
  return h;
}