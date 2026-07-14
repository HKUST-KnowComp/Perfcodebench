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
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    
    size_t i = 0, j = 0;
    size_t left_size = left_keys.size();
    size_t right_size = right_keys.size();
    
    while (i < left_size && j < right_size) {
      if (left_keys[i] < right_keys[j]) {
        hash ^= left_keys[i];
        hash *= FNV_PRIME;
        hash ^= left_values[i];
        hash *= FNV_PRIME;
        ++i;
      } else if (left_keys[i] > right_keys[j]) {
        hash ^= right_keys[j];
        hash *= FNV_PRIME;
        hash ^= right_values[j];
        hash *= FNV_PRIME;
        ++j;
      } else {
        hash ^= right_keys[j];
        hash *= FNV_PRIME;
        hash ^= right_values[j];
        hash *= FNV_PRIME;
        ++i;
        ++j;
      }
    }
    
    while (i < left_size) {
      hash ^= left_keys[i];
      hash *= FNV_PRIME;
      hash ^= left_values[i];
      hash *= FNV_PRIME;
      ++i;
    }
    
    while (j < right_size) {
      hash ^= right_keys[j];
      hash *= FNV_PRIME;
      hash ^= right_values[j];
      hash *= FNV_PRIME;
      ++j;
    }
  }
  
  return hash;
}