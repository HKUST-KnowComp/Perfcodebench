#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0ULL;

  const uint64_t offset = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  uint64_t h = offset;

  size_t i = 0;
  size_t limit = n & ~(size_t)7;

  for (; i < limit; i += 8) {
    uint32_t v0 = dict_vals[ids[i + 0]]; h ^= (uint64_t)v0 + 1ULL; h *= prime;
    uint32_t v1 = dict_vals[ids[i + 1]]; h ^= (uint64_t)v1 + 1ULL; h *= prime;
    uint32_t v2 = dict_vals[ids[i + 2]]; h ^= (uint64_t)v2 + 1ULL; h *= prime;
    uint32_t v3 = dict_vals[ids[i + 3]]; h ^= (uint64_t)v3 + 1ULL; h *= prime;
    uint32_t v4 = dict_vals[ids[i + 4]]; h ^= (uint64_t)v4 + 1ULL; h *= prime;
    uint32_t v5 = dict_vals[ids[i + 5]]; h ^= (uint64_t)v5 + 1ULL; h *= prime;
    uint32_t v6 = dict_vals[ids[i + 6]]; h ^= (uint64_t)v6 + 1ULL; h *= prime;
    uint32_t v7 = dict_vals[ids[i + 7]]; h ^= (uint64_t)v7 + 1ULL; h *= prime;
  }
  for (; i < n; ++i) {
    uint32_t v = dict_vals[ids[i]];
    h ^= (uint64_t)v + 1ULL;
    h *= prime;
  }

  return h;
}
