#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  uint64_t* hist = NULL;
  
  // Allocate histogram once outside the iteration loop
  if (buckets > 0) {
    hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;
  }

  for (int iter = 0; iter < iters; ++iter) {
    // Reset histogram using memset instead of reallocating
    if (buckets > 0) {
      memset(hist, 0, (size_t)buckets * sizeof(uint64_t));
    }

    // Aggregate values into histogram
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }

    // Compute FNV-1a hash over histogram buckets
    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  
  // Free histogram once after all iterations
  free(hist);
  return hash;
}