#pragma once

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  uint64_t hash = 0;
  const auto n = left_keys.size();
  const auto m = right_keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0, j = 0;
    uint32_t last_key = 0xFFFFFFFF;
    uint32_t last_value = 0;
    
    while (i < n || j < m) {
      uint32_t key;
      uint32_t value;
      
      if (i < n && (j >= m || left_keys[i] < right_keys[j])) {
        key = left_keys[i];
        value = left_values[i];
        ++i;
      } else if (j < m && (i >= n || right_keys[j] < left_keys[i])) {
        key = right_keys[j];
        value = right_values[j];
        ++j;
      } else {
        key = left_keys[i];
        value = right_values[j];
        ++i;
        ++j;
      }
      
      if (key == last_key) {
        last_value = value;
      } else {
        hash ^= last_key;
        hash *= 1099511628211ULL;
        hash ^= last_value;
        hash *= 1099511628211ULL;
        last_key = key;
        last_value = value;
      }
    }
    
    if (last_key != 0xFFFFFFFF) {
      hash ^= last_key;
      hash *= 1099511628211ULL;
      hash ^= last_value;
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}