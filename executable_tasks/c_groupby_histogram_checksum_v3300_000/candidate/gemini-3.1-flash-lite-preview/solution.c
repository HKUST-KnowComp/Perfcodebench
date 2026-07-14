#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* __restrict__ keys, const uint32_t* __restrict__ vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    memset(hist, 0, (size_t)buckets * sizeof(uint64_t));
    
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }

    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  
  free(hist);
  return hash;
}