#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* restrict keys, const uint32_t* restrict vals, size_t n, uint32_t buckets, int iters) {
  const size_t hist_size = buckets * sizeof(uint64_t);
  uint64_t* hist = NULL;
  if (buckets > 0) {
    // Align histogram to 64-byte cache lines for optimal vector access
    const size_t aligned_size = (hist_size + 63) & ~63ULL;
    hist = (uint64_t*)aligned_alloc(64, aligned_size);
  }
  if (!hist) return 0;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Fast bulk zeroing of histogram, far cheaper than per-iteration calloc
    memset(hist, 0, hist_size);
    
    // Accumulate values, compiler can auto-vectorize with restrict qualifiers
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }
    
    // Compute identical FNV checksum to preserve correctness
    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }

  free(hist);
  return hash;
}