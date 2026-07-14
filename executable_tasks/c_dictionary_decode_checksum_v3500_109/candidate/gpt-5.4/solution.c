#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;

  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  uint64_t h = 0;

  for (int iter = 0; iter < iters; ++iter) {
    h = fnv_offset;

    size_t i = 0;
    size_t n8 = n & ~(size_t)7;
    for (; i < n8; i += 8) {
      uint64_t v0 = (uint64_t)dict_vals[ids[i + 0]] + 1ULL;
      h ^= v0;
      h *= fnv_prime;

      uint64_t v1 = (uint64_t)dict_vals[ids[i + 1]] + 1ULL;
      h ^= v1;
      h *= fnv_prime;

      uint64_t v2 = (uint64_t)dict_vals[ids[i + 2]] + 1ULL;
      h ^= v2;
      h *= fnv_prime;

      uint64_t v3 = (uint64_t)dict_vals[ids[i + 3]] + 1ULL;
      h ^= v3;
      h *= fnv_prime;

      uint64_t v4 = (uint64_t)dict_vals[ids[i + 4]] + 1ULL;
      h ^= v4;
      h *= fnv_prime;

      uint64_t v5 = (uint64_t)dict_vals[ids[i + 5]] + 1ULL;
      h ^= v5;
      h *= fnv_prime;

      uint64_t v6 = (uint64_t)dict_vals[ids[i + 6]] + 1ULL;
      h ^= v6;
      h *= fnv_prime;

      uint64_t v7 = (uint64_t)dict_vals[ids[i + 7]] + 1ULL;
      h ^= v7;
      h *= fnv_prime;
    }

    for (; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= fnv_prime;
    }
  }

  return h;
}
