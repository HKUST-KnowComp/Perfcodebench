#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t count = 0;
  for (uint64_t word : words) {
    count += static_cast<uint64_t>(__builtin_popcountll(word));
  }
  return count;
}