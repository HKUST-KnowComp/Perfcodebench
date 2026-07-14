#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;

    size_t i = 0;

    for (; i + 8 <= n; i += 8) {
      uint64_t v0 = (uint64_t)dict_vals[ids[i + 0]] + 1ULL;
      hash ^= v0;
      hash *= 1099511628211ULL;

      uint64_t v1 = (uint64_t)dict_vals[ids[i + 1]] + 1ULL;
      hash ^= v1;
      hash *= 1099511628211ULL;

      uint64_t v2 = (uint64_t)dict_vals[ids[i + 2]] + 1ULL;
      hash ^= v2;
      hash *= 1099511628211ULL;

      uint64_t v3 = (uint64_t)dict_vals[ids[i + 3]] + 1ULL;
      hash ^= v3;
      hash *= 1099511628211ULL;

      uint64_t v4 = (uint64_t)dict_vals[ids[i + 4]] + 1ULL;
      hash ^= v4;
      hash *= 1099511628211ULL;

      uint64_t v5 = (uint64_t)dict_vals[ids[i + 5]] + 1ULL;
      hash ^= v5;
      hash *= 1099511628211ULL;

      uint64_t v6 = (uint64_t)dict_vals[ids[i + 6]] + 1ULL;
      hash ^= v6;
      hash *= 1099511628211ULL;

      uint64_t v7 = (uint64_t)dict_vals[ids[i + 7]] + 1ULL;
      hash ^= v7;
      hash *= 1099511628211ULL;
    }

    for (; i < n; ++i) {
      hash ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      hash *= 1099511628211ULL;
    }

    h = hash;
  }

  return h;
}
