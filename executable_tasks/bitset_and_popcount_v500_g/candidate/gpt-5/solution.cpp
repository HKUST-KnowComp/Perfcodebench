#include "interface.h"

#include <vector>
#include <cstddef>

static inline uint64_t popcnt64(uint64_t x) {
  return (uint64_t)__builtin_popcountll(x);
}

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const std::size_t n = (left.size() < right.size()) ? left.size() : right.size();
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* l = left.data();
    const uint64_t* r = right.data();

    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

    std::size_t i = 0;
    const std::size_t n8 = n & ~static_cast<std::size_t>(7);
    for (; i < n8; i += 8) {
      uint64_t w0 = l[i + 0] & r[i + 0];
      uint64_t w1 = l[i + 1] & r[i + 1];
      uint64_t w2 = l[i + 2] & r[i + 2];
      uint64_t w3 = l[i + 3] & r[i + 3];
      uint64_t w4 = l[i + 4] & r[i + 4];
      uint64_t w5 = l[i + 5] & r[i + 5];
      uint64_t w6 = l[i + 6] & r[i + 6];
      uint64_t w7 = l[i + 7] & r[i + 7];

      s0 += popcnt64(w0);
      s1 += popcnt64(w1);
      s2 += popcnt64(w2);
      s3 += popcnt64(w3);
      s0 += popcnt64(w4);
      s1 += popcnt64(w5);
      s2 += popcnt64(w6);
      s3 += popcnt64(w7);
    }
    for (; i < n; ++i) {
      s0 += popcnt64(l[i] & r[i]);
    }

    count = s0 + s1 + s2 + s3;
  }

  return count;
}
