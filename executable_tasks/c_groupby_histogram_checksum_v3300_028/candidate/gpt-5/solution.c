#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t* restrict k = keys;
  const uint32_t* restrict v = vals;
  uint64_t* restrict h = hist;

  // Build histogram once
  size_t i = 0;
  size_t n4 = n & ~(size_t)3;
  for (; i < n4; i += 4) {
    h[k[i + 0]] += (uint64_t)v[i + 0];
    h[k[i + 1]] += (uint64_t)v[i + 1];
    h[k[i + 2]] += (uint64_t)v[i + 2];
    h[k[i + 3]] += (uint64_t)v[i + 3];
  }
  for (; i < n; ++i) {
    h[k[i]] += (uint64_t)v[i];
  }

  // Compute FNV-1a-like checksum across all buckets once
  uint64_t hash = 1469598103934665603ULL;
  uint32_t b = 0;
  uint32_t b4 = buckets & ~3u;
  for (; b < b4; b += 4) {
    hash ^= (h[b + 0] + 1ULL);
    hash *= 1099511628211ULL;
    hash ^= (h[b + 1] + 1ULL);
    hash *= 1099511628211ULL;
    hash ^= (h[b + 2] + 1ULL);
    hash *= 1099511628211ULL;
    hash ^= (h[b + 3] + 1ULL);
    hash *= 1099511628211ULL;
  }
  for (; b < buckets; ++b) {
    hash ^= (h[b] + 1ULL);
    hash *= 1099511628211ULL;
  }

  free(hist);
  return hash;
}
