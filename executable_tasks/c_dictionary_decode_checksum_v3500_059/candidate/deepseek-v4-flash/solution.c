#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      uint32_t v = dict_vals[ids[i]];
      h ^= (uint64_t)v + 1ULL;
      h *= 1099511628211ULL;
    }
  }
  return h;
}
