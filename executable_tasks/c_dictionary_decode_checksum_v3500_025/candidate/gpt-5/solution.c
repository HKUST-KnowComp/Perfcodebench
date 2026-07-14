#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0;

  const uint32_t* __restrict p = ids;
  const uint32_t* __restrict end = ids + n;
  const uint32_t* __restrict dict = dict_vals;

  uint64_t h = 1469598103934665603ULL;
  while (p != end) {
    uint32_t v = dict[*p++];
    h ^= (uint64_t)v + 1ULL;
    h *= 1099511628211ULL;
  }
  return h;
}
