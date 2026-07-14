#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  const uint8_t* __restrict fp = flags.data();
  const uint32_t* __restrict vp = values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // First pass: hash all values where flag == 1 (left partition)
    for (std::size_t i = 0; i < n; ++i) {
      if (fp[i]) {
        hash ^= vp[i];
        hash *= 1099511628211ULL;
      }
    }
    
    // Second pass: hash all values where flag == 0 (right partition)
    for (std::size_t i = 0; i < n; ++i) {
      if (!fp[i]) {
        hash ^= vp[i];
        hash *= 1099511628211ULL;
      }
    }
  }
  
  return hash;
}