#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  std::vector<uint32_t> out(n);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Two-pointer partition: left pointer for true flags, right pointer for false flags
    std::size_t left_pos = 0;
    std::size_t right_pos = 0;
    
    // Count true flags to know where right partition starts
    for (std::size_t i = 0; i < n; ++i) {
      right_pos += flags[i];
    }
    
    std::size_t right_start = right_pos;
    left_pos = 0;
    
    // Partition directly into output
    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        out[left_pos++] = values[i];
      } else {
        out[right_pos++] = values[i];
      }
    }
    
    // Only compute hash on final iteration
    if (iter == iters - 1) {
      hash = 1469598103934665603ULL;
      for (std::size_t i = 0; i < n; ++i) {
        hash = mix(hash, out[i]);
      }
    }
  }
  
  return hash;
}
