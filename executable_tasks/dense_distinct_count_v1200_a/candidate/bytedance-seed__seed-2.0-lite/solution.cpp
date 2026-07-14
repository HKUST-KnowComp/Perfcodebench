#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<bool> seen(key_space, false);
  std::vector<uint32_t> added;
  added.reserve(keys.size());
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    added.clear();
    for (uint32_t key : keys) {
      if (!seen[key]) {
        seen[key] = true;
        added.push_back(key);
      }
    }
    count = static_cast<uint64_t>(added.size());
    // Reset only touched keys to avoid full array clear overhead
    for (uint32_t key : added) {
      seen[key] = false;
    }
  }
  return count;
}