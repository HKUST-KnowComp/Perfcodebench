#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0;

  const uint32_t* p = ids;
  const uint32_t* const end = ids + n;
  uint64_t h = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  for (; end - p >= 4; p += 4) {
    uint32_t v0 = dict_vals[p[0]];
    uint32_t v1 = dict_vals[p[1]];
    uint32_t v2 = dict_vals[p[2]];
    uint32_t v3 = dict_vals[p[3]];

    h ^= (uint64_t)v0 + 1ULL;
    h *= prime;
    h ^= (uint64_t)v1 + 1ULL;
    h *= prime;
    h ^= (uint64_t)v2 + 1ULL;
    h *= prime;
    h ^= (uint64_t)v3 + 1ULL;
    h *= prime;
  }

  for (; p < end; ++p) {
    uint32_t v = dict_vals[*p];
    h ^= (uint64_t)v + 1ULL;
    h *= prime;
  }

  return h;
}
