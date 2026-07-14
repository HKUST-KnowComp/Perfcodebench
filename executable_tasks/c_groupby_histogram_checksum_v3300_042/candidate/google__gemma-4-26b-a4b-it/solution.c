#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (buckets == 0) return 0;
  
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash = 0;
  const size_t bucket_bytes = buckets * sizeof(uint64_t);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset histogram for this iteration
    memset(hist, 0, bucket_bytes);

    // Accumulate values
    size_t i = 0;
    // Manual unrolling to assist ILP
    for (; i + 3 < n; i += 4) {
      hist[keys[i]]     += vals[i];
      hist[keys[i + 1]] += vals[i + 1];
      hist[keys[i + 2]] += vals[i + 2];
      hist[keys[i + 3]] += vals[i + 3];
    }
    for (; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }

    // Compute FNV-1a style checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < buckets; ++b) {
      hash ^= (hist[b] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }

  free(hist);
  return hash;
}