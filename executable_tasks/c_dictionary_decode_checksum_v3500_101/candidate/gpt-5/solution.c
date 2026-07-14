#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0ULL;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t h = FNV_OFFSET;

  const uint32_t* restrict p = ids;
  const uint32_t* restrict end = ids + n;
  const uint32_t* restrict d = dict_vals;

  while (p + 4 <= end) {
    uint32_t id0 = p[0];
    uint32_t id1 = p[1];
    uint32_t id2 = p[2];
    uint32_t id3 = p[3];

    uint64_t v0 = (uint64_t)d[id0] + 1ULL;
    h ^= v0; h *= FNV_PRIME;
    uint64_t v1 = (uint64_t)d[id1] + 1ULL;
    h ^= v1; h *= FNV_PRIME;
    uint64_t v2 = (uint64_t)d[id2] + 1ULL;
    h ^= v2; h *= FNV_PRIME;
    uint64_t v3 = (uint64_t)d[id3] + 1ULL;
    h ^= v3; h *= FNV_PRIME;

    p += 4;
  }

  while (p < end) {
    uint64_t v = (uint64_t)d[*p++] + 1ULL;
    h ^= v;
    h *= FNV_PRIME;
  }

  return h;
}
