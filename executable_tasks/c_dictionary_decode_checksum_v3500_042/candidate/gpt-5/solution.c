#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters == 0) return 0ULL;

  const uint32_t* restrict rid = ids;
  const uint32_t* restrict rdict = dict_vals;

  uint64_t h = 1469598103934665603ULL;

  size_t i = 0;
  size_t n8 = n & ~(size_t)7;
  for (; i < n8; i += 8) {
    uint32_t v0 = rdict[rid[i + 0]]; h ^= (uint64_t)v0 + 1ULL; h *= 1099511628211ULL;
    uint32_t v1 = rdict[rid[i + 1]]; h ^= (uint64_t)v1 + 1ULL; h *= 1099511628211ULL;
    uint32_t v2 = rdict[rid[i + 2]]; h ^= (uint64_t)v2 + 1ULL; h *= 1099511628211ULL;
    uint32_t v3 = rdict[rid[i + 3]]; h ^= (uint64_t)v3 + 1ULL; h *= 1099511628211ULL;
    uint32_t v4 = rdict[rid[i + 4]]; h ^= (uint64_t)v4 + 1ULL; h *= 1099511628211ULL;
    uint32_t v5 = rdict[rid[i + 5]]; h ^= (uint64_t)v5 + 1ULL; h *= 1099511628211ULL;
    uint32_t v6 = rdict[rid[i + 6]]; h ^= (uint64_t)v6 + 1ULL; h *= 1099511628211ULL;
    uint32_t v7 = rdict[rid[i + 7]]; h ^= (uint64_t)v7 + 1ULL; h *= 1099511628211ULL;
  }
  for (; i < n; ++i) {
    uint32_t v = rdict[rid[i]];
    h ^= (uint64_t)v + 1ULL;
    h *= 1099511628211ULL;
  }

  return h;
}
