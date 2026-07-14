#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = left.size() < right.size() ? left.size() : right.size();
  const uint64_t* __restrict__ l = left.data();
  const uint64_t* __restrict__ r = right.data();

  uint64_t count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    count += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
  }
  return count;
}
