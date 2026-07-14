#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t* restrict rkeys = keys;
  const uint32_t* restrict rvals = vals;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
      hist[rkeys[i]]   += rvals[i];
      hist[rkeys[i+1]] += rvals[i+1];
      hist[rkeys[i+2]] += rvals[i+2];
      hist[rkeys[i+3]] += rvals[i+3];
    }
    for (; i < n; ++i) {
      hist[rkeys[i]] += rvals[i];
    }

    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < buckets; ++b) {
      uint64_t v = hist[b];
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
      hist[b] = 0;
    }
  }
  free(hist);
  return hash;
}