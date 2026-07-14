#pragma once

#include <cstdint>
#include <vector>
#include <bitset>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  std::vector<bool> bitmap(key_space, false);
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitmap for each iteration
    std::fill(bitmap.begin(), bitmap.end(), false);
    
    // Mark enabled keys
    for (uint32_t key : enabled_keys) {
      if (key < key_space) {
        bitmap[key] = true;
      }
    }

    uint64_t iter_sum = 0;
    // Process probe keys
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      if (probe_keys[i] < key_space && bitmap[probe_keys[i]]) {
        iter_sum += static_cast<uint64_t>(values[i]);
      }
    }
    total_sum += iter_sum;
  }

  return total_sum;
}