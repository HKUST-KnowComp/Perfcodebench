#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  size_t n = left.size();
  if (right.size() < n) {
    n = right.size();
  }
  if (n == 0) {
    return 0;
  }

  const uint64_t* l = left.data();
  const uint64_t* r = right.data();

  uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
  uint64_t c4 = 0, c5 = 0, c6 = 0, c7 = 0;

  size_t i = 0;
  for (; i + 7 < n; i += 8) {
    c0 += static_cast<uint64_t>(__builtin_popcountll(l[i]     & r[i]));
    c1 += static_cast<uint64_t>(__builtin_popcountll(l[i + 1] & r[i + 1]));
    c2 += static_cast<uint64_t>(__builtin_popcountll(l[i + 2] & r[i + 2]));
    c3 += static_cast<uint64_t>(__builtin_popcountll(l[i + 3] & r[i + 3]));
    c4 += static_cast<uint64_t>(__builtin_popcountll(l[i + 4] & r[i + 4]));
    c5 += static_cast<uint64_t>(__builtin_popcountll(l[i + 5] & r[i + 5]));
    c6 += static_cast<uint64_t>(__builtin_popcountll(l[i + 6] & r[i + 6]));
    c7 += static_cast<uint64_t>(__builtin_popcountll(l[i + 7] & r[i + 7]));
  }

  uint64_t count = c0 + c1 + c2 + c3 + c4 + c5 + c6 + c7;

  for (; i < n; ++i) {
    count += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
  }

  return count;
}
