#include "interface.h"

#include <vector>

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
  const std::size_t n = values.size();
  
  // Pre-allocate buffers once
  std::vector<uint32_t> left;
  std::vector<uint32_t> right;
  left.reserve(n);
  right.reserve(n);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    left.clear();
    right.clear();
    
    // Partition into left and right
    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        left.push_back(values[i]);
      } else {
        right.push_back(values[i]);
      }
    }
    
    // Compute hash directly without creating 'out' vector
    hash = 1469598103934665603ULL;
    for (uint32_t v : left) {
      hash = mix(hash, v);
    }
    for (uint32_t v : right) {
      hash = mix(hash, v);
    }
  }
  
  return hash;
}