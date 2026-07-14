#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  const uint32_t* __restrict__ rid = ids;
  const uint32_t* __restrict__ rdict = dict_vals;
  
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;
    size_t i = 0;
    size_t limit = n - (n % 4);
    
    for (; i < limit; i += 4) {
      uint64_t h0 = h;
      uint64_t h1 = h;
      uint64_t h2 = h;
      uint64_t h3 = h;
      
      h0 ^= (uint64_t)rdict[rid[i]] + 1ULL;
      h1 ^= (uint64_t)rdict[rid[i+1]] + 1ULL;
      h2 ^= (uint64_t)rdict[rid[i+2]] + 1ULL;
      h3 ^= (uint64_t)rdict[rid[i+3]] + 1ULL;
      
      h0 *= 1099511628211ULL;
      h1 *= 1099511628211ULL;
      h2 *= 1099511628211ULL;
      h3 *= 1099511628211ULL;
      
      h = h0 ^ h1 ^ h2 ^ h3;
    }
    
    for (; i < n; ++i) {
      h ^= (uint64_t)rdict[rid[i]] + 1ULL;
      h *= 1099511628211ULL;
    }
  }
  return h;
}