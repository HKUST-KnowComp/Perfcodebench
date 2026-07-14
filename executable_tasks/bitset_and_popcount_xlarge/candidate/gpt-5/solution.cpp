#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  // Process up to the minimum size to avoid out-of-bounds if sizes differ.
  std::size_t n = left.size();
  if (right.size() < n) n = right.size();

  const uint64_t* a_base = left.data();
  const uint64_t* b_base = right.data();

  uint64_t last_count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* a = a_base;
    const uint64_t* b = b_base;

    uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;

    std::size_t i = 0;
    std::size_t limit8 = n & ~static_cast<std::size_t>(7);
    for (; i < limit8; i += 8) {
      c0 += static_cast<uint64_t>(__builtin_popcountll(a[0] & b[0]));
      c1 += static_cast<uint64_t>(__builtin_popcountll(a[1] & b[1]));
      c2 += static_cast<uint64_t>(__builtin_popcountll(a[2] & b[2]));
      c3 += static_cast<uint64_t>(__builtin_popcountll(a[3] & b[3]));
      c0 += static_cast<uint64_t>(__builtin_popcountll(a[4] & b[4]));
      c1 += static_cast<uint64_t>(__builtin_popcountll(a[5] & b[5]));
      c2 += static_cast<uint64_t>(__builtin_popcountll(a[6] & b[6]));
      c3 += static_cast<uint64_t>(__builtin_popcountll(a[7] & b[7]));
      a += 8;
      b += 8;
    }

    for (; i < n; ++i) {
      c0 += static_cast<uint64_t>(__builtin_popcountll(a[0] & b[0]));
      ++a; ++b;
    }

    last_count = (c0 + c1) + (c2 + c3);
  }

  return last_count;
}
