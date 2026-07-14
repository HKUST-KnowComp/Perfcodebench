#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* restrict keys, const uint32_t* restrict vals, size_t n, uint32_t buckets, int iters) {
  (void)iters;
  if (buckets == 0) return 0;
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;
  for (size_t i = 0; i < n; ++i) {
    hist[keys[i]] += vals[i];
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < buckets; ++i) {
    hash ^= (hist[i] + 1ULL);
    hash *= 1099511628211ULL;
  }
  free(hist);
  return hash;
}