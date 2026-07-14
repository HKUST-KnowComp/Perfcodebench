#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;

  const uint64_t kOffset = 1469598103934665603ULL;
  const uint64_t kPrime = 1099511628211ULL;
  uint64_t h = 0;

  for (int iter = 0; iter < iters; ++iter) {
    h = kOffset;

    const uint32_t* p = ids;
    const uint32_t* end4 = ids + (n & ~(size_t)3);

    for (; p < end4; p += 4) {
      uint64_t v0 = (uint64_t)dict_vals[p[0]] + 1ULL;
      h ^= v0;
      h *= kPrime;

      uint64_t v1 = (uint64_t)dict_vals[p[1]] + 1ULL;
      h ^= v1;
      h *= kPrime;

      uint64_t v2 = (uint64_t)dict_vals[p[2]] + 1ULL;
      h ^= v2;
      h *= kPrime;

      uint64_t v3 = (uint64_t)dict_vals[p[3]] + 1ULL;
      h ^= v3;
      h *= kPrime;
    }

    const uint32_t* end = ids + n;
    for (; p < end; ++p) {
      h ^= (uint64_t)dict_vals[*p] + 1ULL;
      h *= kPrime;
    }
  }

  return h;
}
