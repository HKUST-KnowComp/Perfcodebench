#include "interface.h"

#include <vector>
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const size_t n = values.size();
  if (n == 0) {
    uint64_t hash = 1469598103934665603ULL;
    return hash;
  }
  
  // Pre-allocate output buffer once
  std::vector<uint32_t> out(n);
  
  // Count how many go to left partition (flags[i] == true)
  size_t left_count = 0;
  for (size_t i = 0; i < n; ++i) {
    left_count += flags[i];
  }
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Two-pointer approach: fill left from start, right from left_count position
    size_t left_idx = 0;
    size_t right_idx = left_count;
    
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        out[left_idx++] = values[i];
      } else {
        out[right_idx++] = values[i];
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      hash = mix(hash, out[i]);
    }
  }
  
  return hash;
}