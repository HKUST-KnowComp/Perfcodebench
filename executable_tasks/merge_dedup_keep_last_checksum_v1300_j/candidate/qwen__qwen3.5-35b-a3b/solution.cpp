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
  const auto left_size = left_keys.size();
  const auto right_size = right_keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::vector<std::pair<uint32_t, uint32_t>> merged;
    merged.reserve(left_size + right_size);
    
    size_t i = 0, j = 0;
    while (i < left_size && j < right_size) {
      if (left_keys[i] < right_keys[j]) {
        merged.emplace_back(left_keys[i], left_values[i]);
        ++i;
      } else if (left_keys[i] > right_keys[j]) {
        merged.emplace_back(right_keys[j], right_values[j]);
        ++j;
      } else {
        merged.emplace_back(right_keys[j], right_values[j]);
        ++i;
        ++j;
      }
    }
    
    while (i < left_size) {
      merged.emplace_back(left_keys[i], left_values[i]);
      ++i;
    }
    while (j < right_size) {
      merged.emplace_back(right_keys[j], right_values[j]);
      ++j;
    }
    
    hash = 1469598103934665603ULL;
    uint32_t last_key = 0;
    for (const auto& kv : merged) {
      if (kv.first != last_key) {
        hash = mix(hash, kv.first);
        hash = mix(hash, kv.second);
        last_key = kv.first;
      } else {
        hash = mix(hash, kv.second);
      }
    }
  }
  return hash;
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}