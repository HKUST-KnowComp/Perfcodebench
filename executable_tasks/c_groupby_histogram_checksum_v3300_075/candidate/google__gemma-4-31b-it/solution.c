#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (buckets == 0) return 0;
  
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash = 0;
  size_t hist_size = (size_t)buckets * sizeof(uint64_t);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset histogram for this iteration
    memset(hist, 0, hist_size);

    // Aggregate values into buckets
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }

    // Compute FNV-1a style checksum
    uint64_t current_hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      current_hash ^= (hist[i] + 1ULL);
      current_hash *= 1099511628211ULL;
    }
    hash = current_hash;
  }

  free(hist);
  return hash;
}