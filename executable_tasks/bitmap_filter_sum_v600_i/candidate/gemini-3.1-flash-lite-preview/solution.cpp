#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  // Use a vector of bytes as a bitmap for O(1) lookup
  // key_space is the upper bound of keys
  std::vector<uint8_t> bitmap(key_space + 1, 0);
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitmap
    std::fill(bitmap.begin(), bitmap.end(), 0);
    
    // Mark enabled keys
    for (uint32_t key : enabled_keys) {
      if (key <= key_space) {
        bitmap[key] = 1;
      }
    }

    // Calculate sum
    sum = 0;
    const size_t n = probe_keys.size();
    for (size_t i = 0; i < n; ++i) {
      uint32_t key = probe_keys[i];
      if (key <= key_space && bitmap[key]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  
  return sum;
}