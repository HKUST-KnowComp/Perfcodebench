#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t x = 1469598103934665603ULL;

    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      x ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      x *= 1099511628211ULL;

      x ^= (uint64_t)dict_vals[ids[i + 1]] + 1ULL;
      x *= 1099511628211ULL;

      x ^= (uint64_t)dict_vals[ids[i + 2]] + 1ULL;
      x *= 1099511628211ULL;

      x ^= (uint64_t)dict_vals[ids[i + 3]] + 1ULL;
      x *= 1099511628211ULL;
    }

    for (; i < n; ++i) {
      x ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      x *= 1099511628211ULL;
    }

    h = x;
  }

  return h;
}
