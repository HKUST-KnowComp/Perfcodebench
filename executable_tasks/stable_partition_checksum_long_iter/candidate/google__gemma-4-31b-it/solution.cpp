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
  const size_t n = values.size();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    
    // First pass: elements where flags[i] is true (left partition)
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
    
    // Second pass: elements where flags[i] is false (right partition)
    for (size_t i = 0; i < n; ++i) {
      if (!flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
    final_hash = hash;
  }
  
  return final_hash;
}