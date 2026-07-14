#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* restrict ids, size_t n, const uint32_t* restrict dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  const uint64_t prime = 1099511628211ULL;
  const uint64_t offset = 1469598103934665603ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    h = offset;
    for (size_t i = 0; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= prime;
    }
  }
  return h;
}
