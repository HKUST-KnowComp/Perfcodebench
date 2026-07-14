#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  if (iters <= 0) {
    return 0;
  }

  const uint64_t* data = words.data();
  const std::size_t n = words.size();
  uint64_t count = 0;

  std::size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    count += __builtin_popcountll(data[i]);
    count += __builtin_popcountll(data[i + 1]);
    count += __builtin_popcountll(data[i + 2]);
    count += __builtin_popcountll(data[i + 3]);
  }
  for (; i < n; ++i) {
    count += __builtin_popcountll(data[i]);
  }

  return count;
}
