#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const std::size_t n = left.size();
  const uint64_t* const l = left.data();
  const uint64_t* const r = right.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    for (std::size_t i = 0; i < n; ++i) {
      local += static_cast<uint64_t>(__builtin_popcountll(l[i] & r[i]));
    }
    count = local;
  }
  return count;
}
