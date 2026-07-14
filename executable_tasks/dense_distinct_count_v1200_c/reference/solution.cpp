#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<uint8_t> seen(static_cast<std::size_t>(key_space), 0);
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(seen.begin(), seen.end(), 0);
    count = 0;
    for (uint32_t key : keys) {
      uint8_t& slot = seen[static_cast<std::size_t>(key)];
      if (!slot) {
        slot = 1;
        ++count;
      }
    }
  }
  return count;
}
