#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;
    size_t i = 0;
    size_t n_unroll = n & ~3ULL;
    
    // Unrolled loop for better ILP
    for (; i < n_unroll; i += 4) {
      uint64_t h0 = h;
      uint64_t h1, h2, h3;
      
      h0 ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h0 *= FNV_PRIME;
      
      h1 = h0 ^ ((uint64_t)dict_vals[ids[i+1]] + 1ULL);
      h1 *= FNV_PRIME;
      
      h2 = h1 ^ ((uint64_t)dict_vals[ids[i+2]] + 1ULL);
      h2 *= FNV_PRIME;
      
      h3 = h2 ^ ((uint64_t)dict_vals[ids[i+3]] + 1ULL);
      h3 *= FNV_PRIME;
      
      h = h3;
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= FNV_PRIME;
    }
  }
  
  return h;
}