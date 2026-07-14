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
  std::vector<std::pair<uint32_t, uint32_t>> out;
  out.reserve(left_keys.size() + right_keys.size());
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    
    std::size_t i = 0, j = 0;
    const std::size_t left_size = left_keys.size();
    const std::size_t right_size = right_keys.size();
    
    while (i < left_size && j < right_size) {
      uint32_t left_key = left_keys[i];
      uint32_t right_key = right_keys[j];
      
      if (left_key < right_key) {
        out.emplace_back(left_key, left_values[i]);
        ++i;
      } else if (right_key < left_key) {
        out.emplace_back(right_key, right_values[j]);
        ++j;
      } else {
        out.emplace_back(right_key, right_values[j]);
        ++i;
        ++j;
      }
    }
    
    while (i < left_size) {
      out.emplace_back(left_keys[i], left_values[i]);
      ++i;
    }
    
    while (j < right_size) {
      out.emplace_back(right_keys[j], right_values[j]);
      ++j;
    }
    
    hash = 1469598103934665603ULL;
    std::size_t out_size = out.size();
    std::size_t k = 0;
    
    for (; k + 3 < out_size; k += 4) {
      hash = mix(hash, out[k].first);
      hash = mix(hash, out[k].second);
      hash = mix(hash, out[k + 1].first);
      hash = mix(hash, out[k + 1].second);
      hash = mix(hash, out[k + 2].first);
      hash = mix(hash, out[k + 2].second);
      hash = mix(hash, out[k + 3].first);
      hash = mix(hash, out[k + 3].second);
    }
    
    for (; k < out_size; ++k) {
      hash = mix(hash, out[k].first);
      hash = mix(hash, out[k].second);
    }
  }
  
  return hash;
}