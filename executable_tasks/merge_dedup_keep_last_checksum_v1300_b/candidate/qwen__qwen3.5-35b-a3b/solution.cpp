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
  const uint64_t MIX_MULT = 1099511628211ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0, j = 0;
    uint32_t last_key = 0xFFFFFFFF;
    uint32_t last_value = 0;
    
    while (i < left_keys.size() || j < right_keys.size()) {
      uint32_t key;
      uint32_t value;
      
      if (j >= right_keys.size() || (i < left_keys.size() && left_keys[i] < right_keys[j])) {
        key = left_keys[i];
        value = left_values[i];
        ++i;
      } else if (i >= left_keys.size() || right_keys[j] < left_keys[i]) {
        key = right_keys[j];
        value = right_values[j];
        ++j;
      } else {
        key = right_keys[j];
        value = right_values[j];
        ++j;
        ++i;
      }
      
      if (key != last_key) {
        hash ^= last_key;
        hash *= MIX_MULT;
        hash ^= last_value;
        hash *= MIX_MULT;
        last_key = key;
        last_value = value;
      } else {
        last_value = value;
      }
    }
    
    if (last_key != 0xFFFFFFFF) {
      hash ^= last_key;
      hash *= MIX_MULT;
      hash ^= last_value;
      hash *= MIX_MULT;
    }
  }
  
  return hash;
}