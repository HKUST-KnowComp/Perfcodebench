#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  // Allocate histogram once, avoid repeated expensive malloc/free calls
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Build histogram identically to baseline to preserve correctness
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }

    // Exact same FNV-1a checksum calculation to guarantee matching output
    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= 1099511628211ULL;
    }

    // Reset histogram by undoing additions, avoids full per-iteration memset
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] -= vals[i];
    }
  }

  free(hist);
  return hash;
}