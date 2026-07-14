#include "interface.h"
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  (void)iters;
  uint64_t count = 0;
  for (std::size_t i = 0; i < left.size(); ++i) {
    uint64_t w = left[i] & right[i];
    // Use compiler built-in popcount (available in GCC/Clang; MSVC would need <intrin.h>)
    count += __builtin_popcountll(w);
  }
  return count;
}