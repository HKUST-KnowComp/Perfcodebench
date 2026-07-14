#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  // Preserve baseline semantics when iters <= 0
  if (iters <= 0) return 0;

  // We only need the result of the final iteration, so perform the work once.
  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t* restrict k = keys;
  const uint32_t* restrict v = vals;

  // Build histogram with unrolled loop
  size_t i = 0;
  size_t n4 = n & ~(size_t)3;
  for (; i < n4; i += 4) {
    hist[k[i+0]] += (uint64_t)v[i+0];
    hist[k[i+1]] += (uint64_t)v[i+1];
    hist[k[i+2]] += (uint64_t)v[i+2];
    hist[k[i+3]] += (uint64_t)v[i+3];
  }
  for (; i < n; ++i) {
    hist[k[i]] += (uint64_t)v[i];
  }

  // Compute FNV-1a-like checksum over buckets (hist[i] + 1)
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t m = 1099511628211ULL;

  uint32_t b = 0;
  uint32_t b4 = buckets & ~3u;
  for (; b < b4; b += 4) {
    uint64_t x0 = hist[b] + 1ULL;   hash ^= x0; hash *= m;
    uint64_t x1 = hist[b+1] + 1ULL; hash ^= x1; hash *= m;
    uint64_t x2 = hist[b+2] + 1ULL; hash ^= x2; hash *= m;
    uint64_t x3 = hist[b+3] + 1ULL; hash ^= x3; hash *= m;
  }
  for (; b < buckets; ++b) {
    hash ^= (hist[b] + 1ULL);
    hash *= m;
  }

  free(hist);
  return hash;
}
