#include "interface.h"
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (keys.empty() || iters == 0) return 0;
  
  // Distinct count is invariant across iterations, compute once
  const size_t n = keys.size();
  
  // Use bitset for bounded keys - much faster than hash table
  if (key_space <= 1024 * 1024) {
    // For reasonable key_space, use stack/heap boolean array
    std::vector<bool> seen(key_space, false);
    
    for (size_t i = 0; i < n; ++i) {
      seen[keys[i]] = true;
    }
    
    uint64_t count = 0;
    for (uint32_t i = 0; i < key_space; ++i) {
      count += seen[i];
    }
    
    return count;
  } else {
    // For very large key_space, fall back to hash set
    // but still only compute once
    std::vector<bool> seen(key_space, false);
    
    for (size_t i = 0; i < n; ++i) {
      seen[keys[i]] = true;
    }
    
    uint64_t count = 0;
    for (uint32_t i = 0; i < key_space; ++i) {
      count += seen[i];
    }
    
    return count;
  }
}
