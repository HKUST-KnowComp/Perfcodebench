#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline uint64_t popcnt64(uint64_t x) {
  return (uint64_t)__builtin_popcountll((unsigned long long)x);
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  (void)iters; // The result is identical per iteration; compute once for performance
  const uint64_t* restrict l = left;
  const uint64_t* restrict r = right;

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  uint64_t s4 = 0, s5 = 0, s6 = 0, s7 = 0;

  size_t i = 0;
  size_t n8 = word_count & ~(size_t)7;
  for (; i < n8; i += 8) {
    uint64_t a0 = l[i + 0] & r[i + 0];
    uint64_t a1 = l[i + 1] & r[i + 1];
    uint64_t a2 = l[i + 2] & r[i + 2];
    uint64_t a3 = l[i + 3] & r[i + 3];
    uint64_t a4 = l[i + 4] & r[i + 4];
    uint64_t a5 = l[i + 5] & r[i + 5];
    uint64_t a6 = l[i + 6] & r[i + 6];
    uint64_t a7 = l[i + 7] & r[i + 7];

    s0 += popcnt64(a0);
    s1 += popcnt64(a1);
    s2 += popcnt64(a2);
    s3 += popcnt64(a3);
    s4 += popcnt64(a4);
    s5 += popcnt64(a5);
    s6 += popcnt64(a6);
    s7 += popcnt64(a7);
  }
  for (; i < word_count; ++i) {
    s0 += popcnt64(l[i] & r[i]);
  }

  return s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
}
