#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t* restrict k = keys;
  const uint32_t* restrict v = vals;

  // Build histogram once
  size_t i = 0;
  for (; i + 3 < n; i += 4) {
    uint32_t k0 = k[i];
    uint32_t k1 = k[i + 1];
    uint32_t k2 = k[i + 2];
    uint32_t k3 = k[i + 3];
    hist[k0] += v[i];
    hist[k1] += v[i + 1];
    hist[k2] += v[i + 2];
    hist[k3] += v[i + 3];
  }
  for (; i < n; ++i) {
    hist[k[i]] += v[i];
  }

  // Compute checksum once
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  uint32_t b = 0;
  for (; b + 3 < buckets; b += 4) {
    uint64_t h0 = hist[b] + 1ULL;
    hash ^= h0; hash *= prime;
    uint64_t h1 = hist[b + 1] + 1ULL;
    hash ^= h1; hash *= prime;
    uint64_t h2 = hist[b + 2] + 1ULL;
    hash ^= h2; hash *= prime;
    uint64_t h3 = hist[b + 3] + 1ULL;
    hash ^= h3; hash *= prime;
  }
  for (; b < buckets; ++b) {
    hash ^= (hist[b] + 1ULL);
    hash *= prime;
  }

  free(hist);
  return hash;
}
