#include "interface.h"
#include <unordered_set>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)key_space;
  std::unordered_set<uint32_t> seen;
  seen.reserve(keys.size());
  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    uint64_t current = 0;
    for (const uint32_t key : keys) {
      if (seen.insert(key).second) {
        ++current;
      }
    }
    result = current;
  }
  return result;
}