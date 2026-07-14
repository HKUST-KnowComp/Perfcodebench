#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }
    final_hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      final_hash ^= (hist[i] + 1ULL);
      final_hash *= 1099511628211ULL;
      hist[i] = 0;
    }
  }
  free(hist);
  return final_hash;
}