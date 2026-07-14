#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0;

  const uint64_t offset_basis = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  uint64_t h = offset_basis;

  const uint32_t* p = ids;
  const uint32_t* end = ids + n;

  while (p + 8 <= end) {
    uint32_t v0 = dict_vals[p[0]]; h ^= (uint64_t)v0 + 1ULL; h *= prime;
    uint32_t v1 = dict_vals[p[1]]; h ^= (uint64_t)v1 + 1ULL; h *= prime;
    uint32_t v2 = dict_vals[p[2]]; h ^= (uint64_t)v2 + 1ULL; h *= prime;
    uint32_t v3 = dict_vals[p[3]]; h ^= (uint64_t)v3 + 1ULL; h *= prime;
    uint32_t v4 = dict_vals[p[4]]; h ^= (uint64_t)v4 + 1ULL; h *= prime;
    uint32_t v5 = dict_vals[p[5]]; h ^= (uint64_t)v5 + 1ULL; h *= prime;
    uint32_t v6 = dict_vals[p[6]]; h ^= (uint64_t)v6 + 1ULL; h *= prime;
    uint32_t v7 = dict_vals[p[7]]; h ^= (uint64_t)v7 + 1ULL; h *= prime;
    p += 8;
  }
  while (p < end) {
    uint32_t v = dict_vals[*p++];
    h ^= (uint64_t)v + 1ULL;
    h *= prime;
  }

  return h;
}
