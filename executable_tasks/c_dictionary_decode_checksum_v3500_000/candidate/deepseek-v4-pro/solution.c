#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* restrict ids, size_t n,
             const uint32_t* restrict dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  const uint64_t h_init = 1469598103934665603ULL;
  const uint64_t mul = 1099511628211ULL;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    h = h_init;
    size_t i = 0;
    for (; i + 3 < n; i += 4) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1;
      h *= mul;
      h ^= (uint64_t)dict_vals[ids[i+1]] + 1;
      h *= mul;
      h ^= (uint64_t)dict_vals[ids[i+2]] + 1;
      h *= mul;
      h ^= (uint64_t)dict_vals[ids[i+3]] + 1;
      h *= mul;
    }
    for (; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1;
      h *= mul;
    }
  }
  return h;
}
