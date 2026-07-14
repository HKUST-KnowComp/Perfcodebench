#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;
    uint64_t* hist_ptr = hist;
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = keys[i];
      if (key < buckets) {
        hist_ptr[key] += vals[i];
      }
    }
    hash = 1469598103934665603ULL;
    uint64_t* hist_end = hist + buckets;
    for (uint64_t* hist_bucket = hist; hist_bucket < hist_end; ++hist_bucket) {
      hash ^= (*hist_bucket + 1ULL);
      hash *= 1099511628211ULL;
    }
    free(hist);
  }
  return hash;
}