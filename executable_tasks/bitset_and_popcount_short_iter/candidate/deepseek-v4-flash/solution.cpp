#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  if (iters == 0) return 0;
  uint64_t popcount = 0;
  for (std::size_t i = 0; i < left.size(); ++i) {
    popcount += __builtin_popcountll(left[i] & right[i]);
  }
  return popcount * static_cast<uint64_t>(iters);
}
