#include "interface.h"

#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)key_space;
  uint64_t count = 0;
  std::vector<uint32_t> tmp(keys.size());
  for (int iter = 0; iter < iters; ++iter) {
    std::copy(keys.begin(), keys.end(), tmp.begin());
    std::sort(tmp.begin(), tmp.end());
    count = static_cast<uint64_t>(std::unique(tmp.begin(), tmp.end()) - tmp.begin());
  }
  return count;
}
