#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;
    size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    size_t n_unroll = n & ~3ULL;
    for (; i < n_unroll; i += 4) {
      uint64_t v0 = (uint64_t)dict_vals[ids[i]] + 1ULL;
      uint64_t v1 = (uint64_t)dict_vals[ids[i+1]] + 1ULL;
      uint64_t v2 = (uint64_t)dict_vals[ids[i+2]] + 1ULL;
      uint64_t v3 = (uint64_t)dict_vals[ids[i+3]] + 1ULL;
      
      h ^= v0;
      h *= 1099511628211ULL;
      h ^= v1;
      h *= 1099511628211ULL;
      h ^= v2;
      h *= 1099511628211ULL;
      h ^= v3;
      h *= 1099511628211ULL;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= 1099511628211ULL;
    }
  }
  
  return h;
}