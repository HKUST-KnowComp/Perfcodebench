#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (buckets == 0) return 0;
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;
  uint64_t hash = 0;
  if (iters > 0) {
    // First iteration: hist already zeroed from calloc
    for (size_t i = 0; i < n; ++i) hist[keys[i]] += vals[i];
    // Subsequent iterations: reset histogram and accumulate
    for (int iter = 1; iter < iters; ++iter) {
      for (uint32_t j = 0; j < buckets; ++j) hist[j] = 0;
      for (size_t i = 0; i < n; ++i) hist[keys[i]] += vals[i];
    }
    // Compute final checksum
    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  free(hist);
  return hash;
}
