#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (buckets == 0) return 0;
  
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash = 0;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset histogram for next iteration
    memset(hist, 0, buckets * sizeof(uint64_t));

    // Aggregation phase
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }

    // Checksum phase
    hash = FNV_OFFSET;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= FNV_PRIME;
    }
  }

  free(hist);
  return hash;
}