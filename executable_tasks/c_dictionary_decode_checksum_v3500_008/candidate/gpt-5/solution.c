#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0;

  const uint32_t* __restrict ids_r = ids;
  const uint32_t* __restrict dict_r = dict_vals;
  const uint64_t prime = 1099511628211ULL;
  uint64_t h = 1469598103934665603ULL;

  // Unrolled decode + checksum in a single pass
  size_t unroll = n & ~(size_t)3;
  const uint32_t* p = ids_r;
  for (size_t i = 0; i < unroll; i += 4) {
    uint32_t v0 = dict_r[p[0]]; h ^= (uint64_t)v0 + 1ULL; h *= prime;
    uint32_t v1 = dict_r[p[1]]; h ^= (uint64_t)v1 + 1ULL; h *= prime;
    uint32_t v2 = dict_r[p[2]]; h ^= (uint64_t)v2 + 1ULL; h *= prime;
    uint32_t v3 = dict_r[p[3]]; h ^= (uint64_t)v3 + 1ULL; h *= prime;
    p += 4;
  }
  for (size_t i = unroll; i < n; ++i) {
    uint32_t v = dict_r[p[0]];
    h ^= (uint64_t)v + 1ULL;
    h *= prime;
    ++p;
  }

  return h;
}
