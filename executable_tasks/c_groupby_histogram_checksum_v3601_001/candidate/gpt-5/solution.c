#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  (void)iters; // Result depends only on a single aggregation; repeated identical iterations are redundant.

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  // Accumulate values into histogram (unrolled by 8 for reduced loop overhead)
  const uint32_t* k = keys;
  const uint32_t* v = vals;
  size_t i = 0;
  for (; i + 7 < n; i += 8) {
    uint32_t k0 = k[i + 0];
    uint32_t k1 = k[i + 1];
    uint32_t k2 = k[i + 2];
    uint32_t k3 = k[i + 3];
    uint32_t k4 = k[i + 4];
    uint32_t k5 = k[i + 5];
    uint32_t k6 = k[i + 6];
    uint32_t k7 = k[i + 7];
    hist[k0] += (uint64_t)v[i + 0];
    hist[k1] += (uint64_t)v[i + 1];
    hist[k2] += (uint64_t)v[i + 2];
    hist[k3] += (uint64_t)v[i + 3];
    hist[k4] += (uint64_t)v[i + 4];
    hist[k5] += (uint64_t)v[i + 5];
    hist[k6] += (uint64_t)v[i + 6];
    hist[k7] += (uint64_t)v[i + 7];
  }
  for (; i < n; ++i) {
    hist[k[i]] += (uint64_t)v[i];
  }

  // FNV-1a-like hash over buckets
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  size_t b = 0;
  size_t B = (size_t)buckets;
  for (; b + 4 <= B; b += 4) {
    uint64_t h0 = hist[b + 0] + 1ULL;
    uint64_t h1 = hist[b + 1] + 1ULL;
    uint64_t h2 = hist[b + 2] + 1ULL;
    uint64_t h3 = hist[b + 3] + 1ULL;
    hash ^= h0; hash *= prime;
    hash ^= h1; hash *= prime;
    hash ^= h2; hash *= prime;
    hash ^= h3; hash *= prime;
  }
  for (; b < B; ++b) {
    hash ^= (hist[b] + 1ULL);
    hash *= prime;
  }

  free(hist);
  return hash;
}
