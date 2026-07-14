#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t count = 0;
  const uint64_t* data = words.data();
  std::size_t n = words.size();
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (std::size_t i = 0; i < n; ++i) {
      count += static_cast<uint64_t>(__builtin_popcountll(data[i]));
    }
  }
  return count;
}
