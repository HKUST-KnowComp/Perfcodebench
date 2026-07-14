#include "interface.h"

#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t count = 0;
  const std::size_t size = left.size();
  
  for (std::size_t i = 0; i < size; ++i) {
    const uint64_t word = left[i] & right[i];
    count += __builtin_popcountll(word);
  }
  
  return count * static_cast<uint64_t>(iters);
}