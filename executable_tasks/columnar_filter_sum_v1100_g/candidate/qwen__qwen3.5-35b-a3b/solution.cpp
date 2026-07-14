#pragma once

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t total_sum = 0;
  const auto n = keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i] && keys[i] < key_threshold && (keys[i] & 3u) == 0u) {
        iter_sum += static_cast<uint64_t>(values[i]);
      }
    }
    total_sum = iter_sum;
  }
  
  return total_sum;
}