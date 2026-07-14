#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t h = FNV_OFFSET;

  size_t i = 0;
  if (n > 0) {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(ids, 0, 0);
#endif
  }

  size_t n4 = n & ~(size_t)3;

  for (; i < n4; i += 4) {
#if defined(__GNUC__) || defined(__clang__)
    size_t pf = i + 32;
    if (pf < n) {
      uint32_t id_pf = ids[pf];
      __builtin_prefetch((const void*)(dict_vals + id_pf), 0, 0);
      __builtin_prefetch((const void*)(ids + pf + 16), 0, 0);
    }
#endif

    uint32_t v0 = dict_vals[ids[i+0]];
    h ^= (uint64_t)v0 + 1ULL;
    h *= FNV_PRIME;

    uint32_t v1 = dict_vals[ids[i+1]];
    h ^= (uint64_t)v1 + 1ULL;
    h *= FNV_PRIME;

    uint32_t v2 = dict_vals[ids[i+2]];
    h ^= (uint64_t)v2 + 1ULL;
    h *= FNV_PRIME;

    uint32_t v3 = dict_vals[ids[i+3]];
    h ^= (uint64_t)v3 + 1ULL;
    h *= FNV_PRIME;
  }

  for (; i < n; ++i) {
    uint32_t v = dict_vals[ids[i]];
    h ^= (uint64_t)v + 1ULL;
    h *= FNV_PRIME;
  }

  return h;
}
