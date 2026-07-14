#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* restrict keys, const uint32_t* restrict vals, size_t n, uint32_t buckets, int iters) {
  const size_t hist_bytes = buckets * sizeof(uint64_t);
  uint64_t* restrict hist = (uint64_t*)aligned_alloc(64, hist_bytes);
  if (!hist) return 0;
  
  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    memset(hist, 0, hist_bytes);
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }
    hash = fnv_offset;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= fnv_prime;
    }
  }

  free(hist);
  return hash;
}