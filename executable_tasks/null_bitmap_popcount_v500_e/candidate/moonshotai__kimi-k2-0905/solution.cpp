#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (const uint64_t w : words) {
      count += static_cast<uint64_t>(__builtin_popcountll(w));
    }
  }
  return count;
}