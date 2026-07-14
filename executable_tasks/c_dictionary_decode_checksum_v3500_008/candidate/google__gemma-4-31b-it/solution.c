#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  const uint64_t seed = 1469598103934665603ULL;
  const uint64_t multiplier = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    h = seed;
    for (size_t i = 0; i < n; ++i) {
      // Fuse lookup and checksum calculation to avoid intermediate array allocation and memory passes
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= multiplier;
    }
  }
  return h;
}