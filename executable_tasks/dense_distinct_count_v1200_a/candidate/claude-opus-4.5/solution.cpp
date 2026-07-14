#include "interface.h"

#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<char> seen(key_space, 0);
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen.data(), 0, key_space);
    uint64_t c = 0;
    for (uint32_t key : keys) {
      if (!seen[key]) {
        seen[key] = 1;
        ++c;
      }
    }
    count = c;
  }
  return count;
}
