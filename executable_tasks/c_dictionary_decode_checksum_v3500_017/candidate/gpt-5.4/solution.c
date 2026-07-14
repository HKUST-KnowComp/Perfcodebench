#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;

    size_t i = 0;
    size_t n8 = n & ~(size_t)7;
    for (; i < n8; i += 8) {
      hash ^= (uint64_t)dict_vals[ids[i + 0]] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)dict_vals[ids[i + 1]] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)dict_vals[ids[i + 2]] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)dict_vals[ids[i + 3]] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)dict_vals[ids[i + 4]] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)dict_vals[ids[i + 5]] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)dict_vals[ids[i + 6]] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)dict_vals[ids[i + 7]] + 1ULL;
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
