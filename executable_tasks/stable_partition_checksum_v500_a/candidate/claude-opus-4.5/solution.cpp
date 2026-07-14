#include "interface.h"

#include <vector>
#include <cstddef>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  if (n == 0) {
    uint64_t hash = 1469598103934665603ULL;
    return hash;
  }
  
  std::vector<uint32_t> out(n);
  uint32_t* const out_ptr = out.data();
  const uint8_t* const flags_ptr = flags.data();
  const uint32_t* const values_ptr = values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Count how many go to left partition
    std::size_t left_count = 0;
    for (std::size_t i = 0; i < n; ++i) {
      left_count += flags_ptr[i];
    }
    
    // Two-pointer placement: left_idx starts at 0, right_idx starts at left_count
    std::size_t left_idx = 0;
    std::size_t right_idx = left_count;
    
    for (std::size_t i = 0; i < n; ++i) {
      if (flags_ptr[i]) {
        out_ptr[left_idx++] = values_ptr[i];
      } else {
        out_ptr[right_idx++] = values_ptr[i];
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    std::size_t i = 0;
    
    // Unroll by 4
    for (; i + 4 <= n; i += 4) {
      hash ^= out_ptr[i];
      hash *= 1099511628211ULL;
      hash ^= out_ptr[i + 1];
      hash *= 1099511628211ULL;
      hash ^= out_ptr[i + 2];
      hash *= 1099511628211ULL;
      hash ^= out_ptr[i + 3];
      hash *= 1099511628211ULL;
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      hash ^= out_ptr[i];
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}