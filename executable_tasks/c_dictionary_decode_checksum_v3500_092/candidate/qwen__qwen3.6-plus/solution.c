#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  uint64_t h;

  for (int iter = 0; iter < iters; ++iter) {
    h = FNV_OFFSET;
    size_t i = 0;
    for (; i + 7 < n; i += 8) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL; h *= FNV_PRIME;
      h ^= (uint64_t)dict_vals[ids[i+1]] + 1ULL; h *= FNV_PRIME;
      h ^= (uint64_t)dict_vals[ids[i+2]] + 1ULL; h *= FNV_PRIME;
      h ^= (uint64_t)dict_vals[ids[i+3]] + 1ULL; h *= FNV_PRIME;
      h ^= (uint64_t)dict_vals[ids[i+4]] + 1ULL; h *= FNV_PRIME;
      h ^= (uint64_t)dict_vals[ids[i+5]] + 1ULL; h *= FNV_PRIME;
      h ^= (uint64_t)dict_vals[ids[i+6]] + 1ULL; h *= FNV_PRIME;
      h ^= (uint64_t)dict_vals[ids[i+7]] + 1ULL; h *= FNV_PRIME;
    }
    for (; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL; h *= FNV_PRIME;
    }
  }
  return h;
}
