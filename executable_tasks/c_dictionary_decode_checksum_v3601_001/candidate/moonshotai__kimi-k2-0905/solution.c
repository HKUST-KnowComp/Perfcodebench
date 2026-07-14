#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  const uint64_t fnv_prime = 1099511628211ULL;
  const uint64_t fnv_offset = 1469598103934665603ULL;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    h = fnv_offset;
    size_t i = 0;
    size_t remaining = n;

    /* 8-way unroll */
    for (; remaining >= 8; remaining -= 8, i += 8) {
      uint64_t v0 = dict_vals[ids[i + 0]] + 1ULL;
      uint64_t v1 = dict_vals[ids[i + 1]] + 1ULL;
      uint64_t v2 = dict_vals[ids[i + 2]] + 1ULL;
      uint64_t v3 = dict_vals[ids[i + 3]] + 1ULL;
      uint64_t v4 = dict_vals[ids[i + 4]] + 1ULL;
      uint64_t v5 = dict_vals[ids[i + 5]] + 1ULL;
      uint64_t v6 = dict_vals[ids[i + 6]] + 1ULL;
      uint64_t v7 = dict_vals[ids[i + 7]] + 1ULL;

      h ^= v0; h *= fnv_prime;
      h ^= v1; h *= fnv_prime;
      h ^= v2; h *= fnv_prime;
      h ^= v3; h *= fnv_prime;
      h ^= v4; h *= fnv_prime;
      h ^= v5; h *= fnv_prime;
      h ^= v6; h *= fnv_prime;
      h ^= v7; h *= fnv_prime;
    }

    /* tail */
    for (; remaining; --remaining, ++i) {
      h ^= (uint64_t)(dict_vals[ids[i]] + 1ULL);
      h *= fnv_prime;
    }
  }
  return h;
}