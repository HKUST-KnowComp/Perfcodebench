#include "interface.h"
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t final_sum = 0;
  const size_t count = keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < count; ++i) {
      if (flags[i] && keys[i] < key_threshold && (keys[i] & 3U) == 0U) {
        current_sum += static_cast<uint64_t>(values[i]);
      }
    }
    final_sum = current_sum;
  }
  return final_sum;
}