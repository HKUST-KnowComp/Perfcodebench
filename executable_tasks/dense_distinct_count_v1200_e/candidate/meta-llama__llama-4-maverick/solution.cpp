#include "interface.h"

#include <unordered_set>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)key_space;
  std::unordered_set<uint32_t> seen;
  seen.reserve(keys.size());
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    for (uint32_t key : keys) {
      seen.insert(key);
    }
    count = seen.size();
  }
  return count;
}