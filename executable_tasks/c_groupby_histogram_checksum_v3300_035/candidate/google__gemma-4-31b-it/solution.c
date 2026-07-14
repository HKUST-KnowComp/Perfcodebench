#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (buckets == 0) return 0;
  
  uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    memset(hist, 0, buckets * sizeof(uint64_t));
    
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }

    uint64_t h = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      h ^= (hist[i] + 1ULL);
      h *= 1099511628211ULL;
    }
    hash = h;
  }

  free(hist);
  return hash;
}