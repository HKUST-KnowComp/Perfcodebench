#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t count = 0;
  for (int i = 0; i < iters; ++i) {
    uint64_t total = 0;
    for (const auto& word : words) {
      total += static_cast<uint64_t>(__builtin_popcountll(word));
    }
    count = total;
  }
  return count;
}
