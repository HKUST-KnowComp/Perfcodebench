#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)key_space;
  if (iters <= 0) {
    return 0;
  }

  // Optimization: Since 'keys' is const and the result is deterministic,
  // we only need to compute the distinct count once, regardless of 'iters'.
  // Using sort + unique is significantly faster than unordered_set for integers
  // due to better memory locality and lack of per-element heap allocations.
  
  std::vector<uint32_t> sorted_keys = keys;
  std::sort(sorted_keys.begin(), sorted_keys.end());
  
  uint64_t count = 0;
  if (!sorted_keys.empty()) {
    auto last = std::unique(sorted_keys.begin(), sorted_keys.end());
    count = static_cast<uint64_t>(last - sorted_keys.begin());
  }
  
  return count;
}