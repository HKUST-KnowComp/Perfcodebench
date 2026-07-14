#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int /*iters*/) {
  uint64_t count = 0;
  for (std::size_t i = 0; i < left.size(); ++i) {
    count += __builtin_popcountll(left[i] & right[i]);
  }
  return count;
}
