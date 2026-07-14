#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (buckets == 0 || iters == 0) return 0;
  const size_t hist_size = buckets * sizeof(uint64_t);
  const size_t aligned_size = (hist_size + 63) & ~63ULL;
  uint64_t* hist = (uint64_t*)aligned_alloc(64, aligned_size);
  if (!hist) return 0;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    memset(hist, 0, hist_size);
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