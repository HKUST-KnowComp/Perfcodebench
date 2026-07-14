#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  
  // Pre-calculate constants for the FNV-1a hash
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    h = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < n; ++i) {
      // Fuse lookup and hash calculation to avoid extra memory allocation and passes
      uint32_t val = dict_vals[ids[i]];
      h ^= (uint64_t)val + 1ULL;
      h *= FNV_PRIME;
    }
  }
  
  return h;
}