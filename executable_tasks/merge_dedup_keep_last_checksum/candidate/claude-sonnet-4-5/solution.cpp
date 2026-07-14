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
  std::vector<uint32_t> out_keys;
  std::vector<uint32_t> out_values;
  out_keys.reserve(left_keys.size() + right_keys.size());
  out_values.reserve(left_values.size() + right_values.size());
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out_keys.clear();
    out_values.clear();
    
    std::size_t l = 0, r = 0;
    const std::size_t left_size = left_keys.size();
    const std::size_t right_size = right_keys.size();
    
    while (l < left_size && r < right_size) {
      uint32_t left_key = left_keys[l];
      uint32_t right_key = right_keys[r];
      
      if (left_key < right_key) {
        out_keys.push_back(left_key);
        out_values.push_back(left_values[l]);
        ++l;
      } else if (left_key > right_key) {
        out_keys.push_back(right_key);
        out_values.push_back(right_values[r]);
        ++r;
      } else {
        out_keys.push_back(right_key);
        out_values.push_back(right_values[r]);
        ++l;
        ++r;
      }
    }
    
    while (l < left_size) {
      out_keys.push_back(left_keys[l]);
      out_values.push_back(left_values[l]);
      ++l;
    }
    
    while (r < right_size) {
      out_keys.push_back(right_keys[r]);
      out_values.push_back(right_values[r]);
      ++r;
    }
    
    hash = 1469598103934665603ULL;
    const std::size_t out_size = out_keys.size();
    for (std::size_t i = 0; i < out_size; ++i) {
      hash = mix(hash, out_keys[i]);
      hash = mix(hash, out_values[i]);
    }
  }
  
  return hash;
}
