#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  const std::size_t left_size = left_keys.size();
  const std::size_t right_size = right_keys.size();
  const std::size_t max_size = left_size + right_size;
  
  std::vector<uint32_t> out_keys;
  std::vector<uint32_t> out_values;
  out_keys.reserve(max_size);
  out_values.reserve(max_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out_keys.clear();
    out_values.clear();
    
    std::size_t left_idx = 0;
    std::size_t right_idx = 0;
    
    while (left_idx < left_size && right_idx < right_size) {
      uint32_t left_key = left_keys[left_idx];
      uint32_t right_key = right_keys[right_idx];
      
      if (left_key < right_key) {
        out_keys.push_back(left_key);
        out_values.push_back(left_values[left_idx]);
        ++left_idx;
      } else if (right_key < left_key) {
        out_keys.push_back(right_key);
        out_values.push_back(right_values[right_idx]);
        ++right_idx;
      } else {
        out_keys.push_back(right_key);
        out_values.push_back(right_values[right_idx]);
        ++left_idx;
        ++right_idx;
      }
    }
    
    while (left_idx < left_size) {
      out_keys.push_back(left_keys[left_idx]);
      out_values.push_back(left_values[left_idx]);
      ++left_idx;
    }
    
    while (right_idx < right_size) {
      out_keys.push_back(right_keys[right_idx]);
      out_values.push_back(right_values[right_idx]);
      ++right_idx;
    }
    
    hash = 1469598103934665603ULL;
    const std::size_t out_size = out_keys.size();
    std::size_t i = 0;
    
    for (; i + 3 < out_size; i += 4) {
      hash = mix(hash, out_keys[i]);
      hash = mix(hash, out_values[i]);
      hash = mix(hash, out_keys[i + 1]);
      hash = mix(hash, out_values[i + 1]);
      hash = mix(hash, out_keys[i + 2]);
      hash = mix(hash, out_values[i + 2]);
      hash = mix(hash, out_keys[i + 3]);
      hash = mix(hash, out_values[i + 3]);
    }
    
    for (; i < out_size; ++i) {
      hash = mix(hash, out_keys[i]);
      hash = mix(hash, out_values[i]);
    }
  }
  
  return hash;
}