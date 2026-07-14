#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* restrict keys, const uint32_t* restrict vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  if (iters == 0) {
    return hash;
  }
  uint64_t* hist = calloc(buckets, sizeof(uint64_t));
  if (!hist) {
    return 0;
  }
  const uint64_t FNV1A_64_INIT = 1469598103934665603ULL;
  const uint64_t FNV1A_64_PRIME = 1099511628211ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    if (iter != 0) {
      memset(hist, 0, buckets * sizeof(uint64_t));
    }
    
    const uint32_t* k = keys;
    const uint32_t* const k_end = keys + n;
    const uint32_t* v = vals;
    for (; k != k_end; ++k, ++v) {
      hist[*k] += *v;
    }
    
    hash = FNV1A_64_INIT;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= FNV1A_64_PRIME;
    }
  }
  
  free(hist);
  return hash;
}