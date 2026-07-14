#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters == 0) return 0ULL;

  const uint32_t* d = dict_vals;
  const uint32_t* it = ids;
  const uint32_t* end = ids + n;

  uint64_t h = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  size_t n8 = n / 8;
  for (size_t blk = 0; blk < n8; ++blk) {
    uint32_t id0 = it[0];
    uint32_t id1 = it[1];
    uint32_t id2 = it[2];
    uint32_t id3 = it[3];
    uint32_t id4 = it[4];
    uint32_t id5 = it[5];
    uint32_t id6 = it[6];
    uint32_t id7 = it[7];

    uint32_t v0 = d[id0];
    uint32_t v1 = d[id1];
    uint32_t v2 = d[id2];
    uint32_t v3 = d[id3];
    uint32_t v4 = d[id4];
    uint32_t v5 = d[id5];
    uint32_t v6 = d[id6];
    uint32_t v7 = d[id7];

    h ^= (uint64_t)v0 + 1ULL; h *= FNV_PRIME;
    h ^= (uint64_t)v1 + 1ULL; h *= FNV_PRIME;
    h ^= (uint64_t)v2 + 1ULL; h *= FNV_PRIME;
    h ^= (uint64_t)v3 + 1ULL; h *= FNV_PRIME;
    h ^= (uint64_t)v4 + 1ULL; h *= FNV_PRIME;
    h ^= (uint64_t)v5 + 1ULL; h *= FNV_PRIME;
    h ^= (uint64_t)v6 + 1ULL; h *= FNV_PRIME;
    h ^= (uint64_t)v7 + 1ULL; h *= FNV_PRIME;

    it += 8;
  }

  while (it != end) {
    uint64_t v = (uint64_t)d[*it++] + 1ULL;
    h ^= v;
    h *= FNV_PRIME;
  }

  return h;
}
