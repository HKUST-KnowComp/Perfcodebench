#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  std::vector<uint32_t> merged_keys;
  std::vector<uint32_t> merged_values;
  merged_keys.reserve(left_keys.size() + right_keys.size());
  merged_values.reserve(left_keys.size() + right_keys.size());
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    merged_keys.clear();
    merged_values.clear();
    
    // Two-pointer merge with keep-last semantics
    std::size_t i = 0, j = 0;
    const std::size_t left_size = left_keys.size();
    const std::size_t right_size = right_keys.size();
    
    while (i < left_size && j < right_size) {
      uint32_t left_key = left_keys[i];
      uint32_t right_key = right_keys[j];
      
      if (left_key < right_key) {
        merged_keys.push_back(left_key);
        merged_values.push_back(left_values[i]);
        ++i;
      } else if (left_key > right_key) {
        merged_keys.push_back(right_key);
        merged_values.push_back(right_values[j]);
        ++j;
      } else {
        // Duplicate key: keep right value (keep-last semantics)
        merged_keys.push_back(right_key);
        merged_values.push_back(right_values[j]);
        ++i;
        ++j;
      }
    }
    
    // Append remaining elements from left
    while (i < left_size) {
      merged_keys.push_back(left_keys[i]);
      merged_values.push_back(left_values[i]);
      ++i;
    }
    
    // Append remaining elements from right
    while (j < right_size) {
      merged_keys.push_back(right_keys[j]);
      merged_values.push_back(right_values[j]);
      ++j;
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const std::size_t merged_size = merged_keys.size();
    for (std::size_t k = 0; k < merged_size; ++k) {
      hash = mix(hash, merged_keys[k]);
      hash = mix(hash, merged_values[k]);
    }
  }
  
  return hash;
}