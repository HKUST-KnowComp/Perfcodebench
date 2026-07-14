#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset hash for each iteration as per original logic
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      // Directly access dict_vals to avoid the extra memory write/read of the 'decoded' array
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= prime;
    }
  }
  return h;
}