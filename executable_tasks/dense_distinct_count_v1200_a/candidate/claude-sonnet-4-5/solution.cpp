#include "interface.h"

#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<bool> seen(key_space, false);
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset bitset
    std::fill(seen.begin(), seen.end(), false);
    
    // Mark seen keys
    for (uint32_t key : keys) {
      seen[key] = true;
    }
    
    // Count distinct
    count = 0;
    for (uint32_t i = 0; i < key_space; ++i) {
      if (seen[i]) {
        ++count;
      }
    }
  }
  
  return count;
}