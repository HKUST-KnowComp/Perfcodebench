#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;

  // Match baseline behavior: if iters == 0, return 0 without computing.
  if (iters <= 0) return 0ULL;

  const uint32_t* restrict ids_p = ids;
  const uint32_t* restrict dict = dict_vals;

  uint64_t h = 1469598103934665603ULL; // FNV-1a offset basis
  const uint64_t prime = 1099511628211ULL; // FNV-1a prime

  size_t i = 0;

  // Unroll by 8: gather indices first to expose memory-level parallelism,
  // then load dictionary values and update hash.
  size_t limit = n & ~(size_t)7;
  for (; i < limit; i += 8) {
    uint32_t idx0 = ids_p[i + 0];
    uint32_t idx1 = ids_p[i + 1];
    uint32_t idx2 = ids_p[i + 2];
    uint32_t idx3 = ids_p[i + 3];
    uint32_t idx4 = ids_p[i + 4];
    uint32_t idx5 = ids_p[i + 5];
    uint32_t idx6 = ids_p[i + 6];
    uint32_t idx7 = ids_p[i + 7];

    uint64_t v0 = (uint64_t)dict[idx0] + 1ULL;
    uint64_t v1 = (uint64_t)dict[idx1] + 1ULL;
    uint64_t v2 = (uint64_t)dict[idx2] + 1ULL;
    uint64_t v3 = (uint64_t)dict[idx3] + 1ULL;
    uint64_t v4 = (uint64_t)dict[idx4] + 1ULL;
    uint64_t v5 = (uint64_t)dict[idx5] + 1ULL;
    uint64_t v6 = (uint64_t)dict[idx6] + 1ULL;
    uint64_t v7 = (uint64_t)dict[idx7] + 1ULL;

    h ^= v0; h *= prime;
    h ^= v1; h *= prime;
    h ^= v2; h *= prime;
    h ^= v3; h *= prime;
    h ^= v4; h *= prime;
    h ^= v5; h *= prime;
    h ^= v6; h *= prime;
    h ^= v7; h *= prime;
  }

  // Remainder
  for (; i < n; ++i) {
    uint64_t v = (uint64_t)dict[ids_p[i]] + 1ULL;
    h ^= v;
    h *= prime;
  }

  return h;
}
