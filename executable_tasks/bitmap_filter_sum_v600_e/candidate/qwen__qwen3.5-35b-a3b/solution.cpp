#pragma once

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  std::vector<bool> bitmap(key_space, false);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitmap
    std::fill(bitmap.begin(), bitmap.end(), false);
    
    // Mark enabled keys
    for (uint32_t key : enabled_keys) {
      if (key < key_space) {
        bitmap[key] = true;
      }
    }
    
    // Compute sum for this iteration
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      uint32_t key = probe_keys[i];
      if (key < key_space && bitmap[key]) {
        current_sum += static_cast<uint64_t>(values[i]);
      }
    }
    
    sum = current_sum;
  }
  
  return sum;
}