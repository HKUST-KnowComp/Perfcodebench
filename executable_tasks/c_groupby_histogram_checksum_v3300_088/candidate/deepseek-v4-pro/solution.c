#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* k = keys;
    const uint32_t* v = vals;
    for (size_t i = 0; i < n; ++i) {
      hist[k[i]] += v[i];
    }
    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      uint64_t val = hist[i];
      hash ^= (val + 1ULL);
      hash *= 1099511628211ULL;
      hist[i] = 0;
    }
  }
  free(hist);
  return hash;
}