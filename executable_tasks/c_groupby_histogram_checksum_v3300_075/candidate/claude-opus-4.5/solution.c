#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    if (iter > 0) {
      memset(hist, 0, buckets * sizeof(uint64_t));
    }
    
    const uint32_t* restrict k = keys;
    const uint32_t* restrict v = vals;
    
    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    
    for (; i < n4; i += 4) {
      hist[k[i]]     += v[i];
      hist[k[i + 1]] += v[i + 1];
      hist[k[i + 2]] += v[i + 2];
      hist[k[i + 3]] += v[i + 3];
    }
    
    for (; i < n; ++i) {
      hist[k[i]] += v[i];
    }
    
    hash = 1469598103934665603ULL;
    uint32_t j = 0;
    uint32_t b4 = buckets & ~(uint32_t)3;
    
    for (; j < b4; j += 4) {
      hash ^= (hist[j] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[j + 1] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[j + 2] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[j + 3] + 1ULL);
      hash *= 1099511628211ULL;
    }
    
    for (; j < buckets; ++j) {
      hash ^= (hist[j] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  
  free(hist);
  return hash;
}