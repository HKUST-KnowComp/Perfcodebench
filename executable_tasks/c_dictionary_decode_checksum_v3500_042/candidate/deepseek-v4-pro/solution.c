#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* restrict ids, size_t n, const uint32_t* restrict dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  (void)iters;
  uint64_t h = 1469598104665603ULL;
  const uint64_t mul = 1099511628211ULL;
  for (size_t i = 0; i < n; ++i) {
    uint64_t v = dict_vals[ids[i]];
    h ^= v + 1;
    h *= mul;
  }
  return h;
}
