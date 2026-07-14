#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  const uint64_t FNV_offset = 1469598103934665603ULL;
  const uint64_t FNV_prime  = 1099511628211ULL;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    h = FNV_offset;
    size_t i = 0;
    /* process 8 ids per iteration */
    for (; i + 7 < n; i += 8) {
      uint32_t v0 = dict_vals[ids[i+0]];
      uint32_t v1 = dict_vals[ids[i+1]];
      uint32_t v2 = dict_vals[ids[i+2]];
      uint32_t v3 = dict_vals[ids[i+3]];
      uint32_t v4 = dict_vals[ids[i+4]];
      uint32_t v5 = dict_vals[ids[i+5]];
      uint32_t v6 = dict_vals[ids[i+6]];
      uint32_t v7 = dict_vals[ids[i+7]];

      h ^= (uint64_t)(v0 + 1); h *= FNV_prime;
      h ^= (uint64_t)(v1 + 1); h *= FNV_prime;
      h ^= (uint64_t)(v2 + 1); h *= FNV_prime;
      h ^= (uint64_t)(v3 + 1); h *= FNV_prime;
      h ^= (uint64_t)(v4 + 1); h *= FNV_prime;
      h ^= (uint64_t)(v5 + 1); h *= FNV_prime;
      h ^= (uint64_t)(v6 + 1); h *= FNV_prime;
      h ^= (uint64_t)(v7 + 1); h *= FNV_prime;
    }
    /* tail */
    for (; i < n; ++i) {
      uint32_t v = dict_vals[ids[i]];
      h ^= (uint64_t)(v + 1);
      h *= FNV_prime;
    }
  }
  return h;
}