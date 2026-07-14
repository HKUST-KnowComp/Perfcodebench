#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  const uint64_t prime = 1099511628211ULL;
  const uint64_t offset = 1469598103934665603ULL;

  uint64_t h = offset;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = offset;
    size_t i = 0;

    /* 4-way unrolled hot path */
    for (; i + 3 < n; i += 4) {
      uint64_t v0 = dict_vals[ids[i  ]] + 1ULL;
      uint64_t v1 = dict_vals[ids[i+1]] + 1ULL;
      uint64_t v2 = dict_vals[ids[i+2]] + 1ULL;
      uint64_t v3 = dict_vals[ids[i+3]] + 1ULL;

      local ^= v0; local *= prime;
      local ^= v1; local *= prime;
      local ^= v2; local *= prime;
      local ^= v3; local *= prime;
    }

    /* tail */
    for (; i < n; ++i) {
      local ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      local *= prime;
    }

    h = local;
  }
  return h;
}