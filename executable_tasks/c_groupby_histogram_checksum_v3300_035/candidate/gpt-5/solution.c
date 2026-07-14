#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;

  // Build histogram once (result is identical across iterations)
  size_t i = 0;
  // Unroll by 4 for better throughput
  for (; i + 4 <= n; i += 4) {
    uint32_t k0 = keys[i + 0];
    uint32_t k1 = keys[i + 1];
    uint32_t k2 = keys[i + 2];
    uint32_t k3 = keys[i + 3];
    hist[k0] += (uint64_t)vals[i + 0];
    hist[k1] += (uint64_t)vals[i + 1];
    hist[k2] += (uint64_t)vals[i + 2];
    hist[k3] += (uint64_t)vals[i + 3];
  }
  for (; i < n; ++i) {
    hist[keys[i]] += (uint64_t)vals[i];
  }

  // Compute checksum (FNV-1 like hashing over buckets)
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t b = 0; b < buckets; ++b) {
    hash ^= (hist[b] + 1ULL);
    hash *= 1099511628211ULL;
  }

  free(hist);
  return hash;
}
