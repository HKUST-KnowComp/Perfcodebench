#include "interface.h"

#include <unordered_set>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  uint64_t count = 0;
  
  // Use bitset approach for reasonable key_space sizes
  if (key_space <= 100000000) {
    std::vector<bool> seen(key_space, false);
    
    for (int iter = 0; iter < iters; ++iter) {
      // Clear bitset
      std::fill(seen.begin(), seen.end(), false);
      
      // Mark seen keys
      for (uint32_t key : keys) {
        seen[key] = true;
      }
      
      // Count distinct
      count = 0;
      for (bool bit : seen) {
        count += bit;
      }
    }
  } else {
    // Fall back to hash set for very large key spaces
    std::unordered_set<uint32_t> seen;
    seen.reserve(keys.size());
    
    for (int iter = 0; iter < iters; ++iter) {
      seen.clear();
      for (uint32_t key : keys) {
        seen.insert(key);
      }
      count = static_cast<uint64_t>(seen.size());
    }
  }
  
  return count;
}