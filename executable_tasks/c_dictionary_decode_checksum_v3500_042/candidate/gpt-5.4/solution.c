#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;

  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  uint64_t h = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t x = fnv_offset;
    size_t i = 0;

    for (; i + 7 < n; i += 8) {
      x ^= (uint64_t)dict_vals[ids[i + 0]] + 1ULL;
      x *= fnv_prime;
      x ^= (uint64_t)dict_vals[ids[i + 1]] + 1ULL;
      x *= fnv_prime;
      x ^= (uint64_t)dict_vals[ids[i + 2]] + 1ULL;
      x *= fnv_prime;
      x ^= (uint64_t)dict_vals[ids[i + 3]] + 1ULL;
      x *= fnv_prime;
      x ^= (uint64_t)dict_vals[ids[i + 4]] + 1ULL;
      x *= fnv_prime;
      x ^= (uint64_t)dict_vals[ids[i + 5]] + 1ULL;
      x *= fnv_prime;
      x ^= (uint64_t)dict_vals[ids[i + 6]] + 1ULL;
      x *= fnv_prime;
      x ^= (uint64_t)dict_vals[ids[i + 7]] + 1ULL;
      x *= fnv_prime;
    }

    for (; i < n; ++i) {
      x ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      x *= fnv_prime;
    }

    h = x;
  }

  return h;
}
