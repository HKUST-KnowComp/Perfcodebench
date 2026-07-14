#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;

  const uint32_t* restrict ids_r = ids;
  const uint32_t* restrict dict_r = dict_vals;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;

    const uint32_t* p = ids_r;
    const uint32_t* end = p + n;

    // Unroll by 4 for reduced loop overhead
    while ((size_t)(end - p) >= 4) {
      uint32_t v0 = dict_r[p[0]];
      h ^= (uint64_t)v0 + 1ULL;
      h *= 1099511628211ULL;

      uint32_t v1 = dict_r[p[1]];
      h ^= (uint64_t)v1 + 1ULL;
      h *= 1099511628211ULL;

      uint32_t v2 = dict_r[p[2]];
      h ^= (uint64_t)v2 + 1ULL;
      h *= 1099511628211ULL;

      uint32_t v3 = dict_r[p[3]];
      h ^= (uint64_t)v3 + 1ULL;
      h *= 1099511628211ULL;

      p += 4;
    }

    while (p < end) {
      uint32_t v = dict_r[*p++];
      h ^= (uint64_t)v + 1ULL;
      h *= 1099511628211ULL;
    }
  }

  return h;
}
