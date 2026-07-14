#include "interface.h"
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
#define PCNT64(x) ((uint64_t)__builtin_popcountll((unsigned long long)(x)))
#else
static inline uint64_t PCNT64(uint64_t x) {
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (x * 0x0101010101010101ULL) >> 56;
}
#endif

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t result = 0;
  if (iters <= 0 || word_count == 0) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t *l = left;
    const uint64_t *r = right;
    size_t n = word_count;
    uint64_t total = 0;

    while (n >= 8) {
      uint64_t w0 = l[0] & r[0];
      uint64_t w1 = l[1] & r[1];
      uint64_t w2 = l[2] & r[2];
      uint64_t w3 = l[3] & r[3];
      uint64_t w4 = l[4] & r[4];
      uint64_t w5 = l[5] & r[5];
      uint64_t w6 = l[6] & r[6];
      uint64_t w7 = l[7] & r[7];
      total += PCNT64(w0);
      total += PCNT64(w1);
      total += PCNT64(w2);
      total += PCNT64(w3);
      total += PCNT64(w4);
      total += PCNT64(w5);
      total += PCNT64(w6);
      total += PCNT64(w7);
      l += 8;
      r += 8;
      n -= 8;
    }

    while (n--) {
      total += PCNT64((*l++) & (*r++));
    }

    result = total; // same value each iteration; keep semantics of baseline
  }

  return result;
}
