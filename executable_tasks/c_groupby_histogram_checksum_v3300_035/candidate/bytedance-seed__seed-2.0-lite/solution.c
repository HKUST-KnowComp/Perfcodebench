#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  if (iters <= 0) return hash;
  
  // Allocate histogram once, reuse across all iterations
  uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
  if (!hist) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Zero buffer to match calloc initialization, far faster than reallocating
    memset(hist, 0, buckets * sizeof(uint64_t));
    
    // Aggregate values into histogram, same logic as original
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }
    
    // Exact same FNV1a checksum calculation to preserve correctness
    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  
  free(hist);
  return hash;
}