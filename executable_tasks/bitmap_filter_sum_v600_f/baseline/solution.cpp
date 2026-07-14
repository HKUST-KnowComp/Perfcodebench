#include "interface.h"

#include <unordered_set>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  (void)key_space;
  std::unordered_set<uint32_t> enabled;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    enabled.clear();
    enabled.insert(enabled_keys.begin(), enabled_keys.end());
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      if (enabled.find(probe_keys[i]) != enabled.end()) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}
