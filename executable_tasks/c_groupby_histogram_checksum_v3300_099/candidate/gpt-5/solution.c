#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t* restrict k = keys;
  const uint32_t* restrict v = vals;

  // Aggregate values into histogram
  size_t i = 0;
  size_t n4 = n & ~(size_t)3;
  for (; i < n4; i += 4) {
    uint32_t k0 = k[i];
    uint32_t k1 = k[i + 1];
    uint32_t k2 = k[i + 2];
    uint32_t k3 = k[i + 3];
    hist[k0] += (uint64_t)v[i];
    hist[k1] += (uint64_t)v[i + 1];
    hist[k2] += (uint64_t)v[i + 2];
    hist[k3] += (uint64_t)v[i + 3];
  }
  for (; i < n; ++i) {
    hist[k[i]] += (uint64_t)v[i];
  }

  // Compute FNV-1a hash over buckets of (hist[i] + 1)
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  uint32_t b = 0;
  uint32_t b4 = buckets & ~3u;
  for (; b < b4; b += 4) {
    uint64_t x0 = hist[b] + 1ULL;
    uint64_t x1 = hist[b + 1] + 1ULL;
    uint64_t x2 = hist[b + 2] + 1ULL;
    uint64_t x3 = hist[b + 3] + 1ULL;
    hash ^= x0; hash *= prime;
    hash ^= x1; hash *= prime;
    hash ^= x2; hash *= prime;
    hash ^= x3; hash *= prime;
  }
  for (; b < buckets; ++b) {
    uint64_t x = hist[b] + 1ULL;
    hash ^= x;
    hash *= prime;
  }

  free(hist);
  return hash;
}
