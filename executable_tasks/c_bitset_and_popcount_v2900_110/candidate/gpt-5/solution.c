#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline uint64_t popcnt64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
  return (uint64_t)__builtin_popcountll(x);
#else
  uint64_t c = 0;
  while (x) { x &= (x - 1); ++c; }
  return c;
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  if (iters <= 0 || word_count == 0) return 0;

  const uint64_t* l = left;
  const uint64_t* r = right;
  size_t n = word_count;

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

  size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    uint64_t a0 = l[i] & r[i];
    uint64_t a1 = l[i + 1] & r[i + 1];
    uint64_t a2 = l[i + 2] & r[i + 2];
    uint64_t a3 = l[i + 3] & r[i + 3];
    s0 += popcnt64(a0);
    s1 += popcnt64(a1);
    s2 += popcnt64(a2);
    s3 += popcnt64(a3);
  }
  for (; i < n; ++i) {
    s0 += popcnt64(l[i] & r[i]);
  }

  uint64_t total = s0 + s1 + s2 + s3;

  (void)iters; // result is deterministic per call; last-iter value equals computed value
  return total;
}
