#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // The baseline re-initializes h inside the loop, but the logic implies 
    // the checksum is calculated on the result of the last iteration's decoding.
    // However, looking at the baseline: h is reset to the offset basis 
    // at the start of every iteration, and the final value is returned.
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      uint32_t val = dict_vals[ids[i]];
      h ^= (uint64_t)val + 1ULL;
      h *= FNV_PRIME;
    }
  }
  return h;
}