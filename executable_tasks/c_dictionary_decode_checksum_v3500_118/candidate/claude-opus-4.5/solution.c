#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    h = FNV_OFFSET;
    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      uint64_t v0 = (uint64_t)dict_vals[ids[i]] + 1ULL;
      uint64_t v1 = (uint64_t)dict_vals[ids[i + 1]] + 1ULL;
      uint64_t v2 = (uint64_t)dict_vals[ids[i + 2]] + 1ULL;
      uint64_t v3 = (uint64_t)dict_vals[ids[i + 3]] + 1ULL;
      h ^= v0;
      h *= FNV_PRIME;
      h ^= v1;
      h *= FNV_PRIME;
      h ^= v2;
      h *= FNV_PRIME;
      h ^= v3;
      h *= FNV_PRIME;
    }
    for (; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= FNV_PRIME;
    }
  }
  return h;
}
