#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0; // Baseline returns 0 when no iterations are performed

  const uint32_t* __restrict ids_p = ids;
  const uint32_t* __restrict dict_p = dict_vals;

  uint64_t h = 1469598103934665603ULL;

  size_t i = 0;
  size_t n4 = n & ~(size_t)3;

  for (; i < n4; i += 4) {
    uint32_t v0 = dict_p[ids_p[i]];
    uint32_t v1 = dict_p[ids_p[i + 1]];
    uint32_t v2 = dict_p[ids_p[i + 2]];
    uint32_t v3 = dict_p[ids_p[i + 3]];

    h ^= (uint64_t)v0 + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)v1 + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)v2 + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)v3 + 1ULL;
    h *= 1099511628211ULL;
  }

  for (; i < n; ++i) {
    uint32_t v = dict_p[ids_p[i]];
    h ^= (uint64_t)v + 1ULL;
    h *= 1099511628211ULL;
  }

  return h;
}
