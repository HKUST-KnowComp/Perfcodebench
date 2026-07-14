#include "interface.h"

#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<char> seen(key_space, 0);
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(seen.begin(), seen.end(), 0);
    for (uint32_t key : keys) {
      seen[key] = 1;
    }
    count = 0;
    for (char c : seen) {
      count += c;
    }
  }
  return count;
}