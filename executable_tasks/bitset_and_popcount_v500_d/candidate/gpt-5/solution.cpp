#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  const uint64_t* L = left.data();
  const uint64_t* R = right.data();
  const std::size_t n = left.size();

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0;
    std::size_t i = 0;

    // Unrolled loop for better ILP and reduced loop overhead
    for (; i + 7 < n; i += 8) {
      s0 += static_cast<uint64_t>(__builtin_popcountll(L[i + 0] & R[i + 0]));
      s1 += static_cast<uint64_t>(__builtin_popcountll(L[i + 1] & R[i + 1]));
      s2 += static_cast<uint64_t>(__builtin_popcountll(L[i + 2] & R[i + 2]));
      s3 += static_cast<uint64_t>(__builtin_popcountll(L[i + 3] & R[i + 3]));
      s4 += static_cast<uint64_t>(__builtin_popcountll(L[i + 4] & R[i + 4]));
      s5 += static_cast<uint64_t>(__builtin_popcountll(L[i + 5] & R[i + 5]));
      s6 += static_cast<uint64_t>(__builtin_popcountll(L[i + 6] & R[i + 6]));
      s7 += static_cast<uint64_t>(__builtin_popcountll(L[i + 7] & R[i + 7]));
    }

    uint64_t s = ((s0 + s1) + (s2 + s3)) + ((s4 + s5) + (s6 + s7));

    for (; i < n; ++i) {
      s += static_cast<uint64_t>(__builtin_popcountll(L[i] & R[i]));
    }

    count = s; // Return last iteration's result (matches baseline semantics)
  }

  return count;
}
