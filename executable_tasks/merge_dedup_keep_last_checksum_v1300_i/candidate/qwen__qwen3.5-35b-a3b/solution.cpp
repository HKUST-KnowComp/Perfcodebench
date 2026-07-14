#pragma once

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  
  uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
  }

  std::vector<std::pair<uint32_t, uint32_t>> merged;
  merged.reserve(left_keys.size() + right_keys.size());
  
  for (int iter = 0; iter < iters; ++iter) {
    merged.clear();
    
    size_t i = 0, j = 0;
    while (i < left_keys.size() && j < right_keys.size()) {
      if (left_keys[i] < right_keys[j]) {
        merged.push_back({left_keys[i], left_values[i]});
        ++i;
      } else if (left_keys[i] > right_keys[j]) {
        merged.push_back({right_keys[j], right_values[j]});
        ++j;
      } else {
        // Equal keys: add left first, then right (preserves order for stable dedup)
        merged.push_back({left_keys[i], left_values[i]});
        ++i;
        // Don't increment j yet - will be handled in next iteration
      }
    }
    
    // Add remaining elements
    while (i < left_keys.size()) {
      merged.push_back({left_keys[i], left_values[i]});
      ++i;
    }
    while (j < right_keys.size()) {
      merged.push_back({right_keys[j], right_values[j]});
      ++j;
    }
    
    // Deduplicate in-place, keeping last occurrence
    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < merged.size(); ++read_idx) {
      if (read_idx == 0 || merged[read_idx].first != merged[read_idx - 1].first) {
        merged[write_idx++] = merged[read_idx];
      } else {
        // Update value for duplicate key (keep right-side value)
        merged[write_idx - 1].second = merged[read_idx].second;
      }
    }
    merged.resize(write_idx);
    
    // Compute hash
    uint64_t hash = 1469598103934665603ULL;
    for (const auto& kv : merged) {
      hash = mix(hash, kv.first);
      hash = mix(hash, kv.second);
    }
  }
  
  return hash;
}