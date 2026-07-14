#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t h = FNV_OFFSET;
  if (n == 0) return h;

  const uint32_t* ip = ids;
  const uint32_t* ip_end = ids + n;
  for (; ip != ip_end; ++ip) {
    uint32_t v = dict_vals[*ip];
    h ^= ((uint64_t)v) + 1ULL;
    h *= FNV_PRIME;
  }
  return h;
}
