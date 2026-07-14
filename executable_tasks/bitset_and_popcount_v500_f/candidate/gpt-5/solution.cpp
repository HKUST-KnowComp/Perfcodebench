#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const std::size_t n = left.size();
  const uint64_t* l = left.data();
  const uint64_t* r = right.data();

  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    std::size_t i = 0;

    // Unroll by 8 for improved throughput
    for (; i + 7 < n; i += 8) {
      uint64_t w0 = l[i] & r[i];
      uint64_t w1 = l[i + 1] & r[i + 1];
      uint64_t w2 = l[i + 2] & r[i + 2];
      uint64_t w3 = l[i + 3] & r[i + 3];
      uint64_t w4 = l[i + 4] & r[i + 4];
      uint64_t w5 = l[i + 5] & r[i + 5];
      uint64_t w6 = l[i + 6] & r[i + 6];
      uint64_t w7 = l[i + 7] & r[i + 7];

      count += static_cast<uint64_t>(__builtin_popcountll(w0));
      count += static_cast<uint64_t>(__builtin_popcountll(w1));
      count += static_cast<uint64_t>(__builtin_popcountll(w2));
      count += static_cast<uint64_t>(__builtin_popcountll(w3));
      count += static_cast<uint64_t>(__builtin_popcountll(w4));
      count += static_cast<uint64_t>(__builtin_popcountll(w5));
      count += static_cast<uint64_t>(__builtin_popcountll(w6));
      count += static_cast<uint64_t>(__builtin_popcountll(w7));
    }

    for (; i < n; ++i) {
      uint64_t w = l[i] & r[i];
      count += static_cast<uint64_t>(__builtin_popcountll(w));
    }

    result = count;
  }

  return result;
}
