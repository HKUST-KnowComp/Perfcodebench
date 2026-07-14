#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  
  const uint32_t* lk = left_keys.data();
  const uint32_t* lv = left_values.data();
  const uint32_t* rk = right_keys.data();
  const uint32_t* rv = right_values.data();
  const std::size_t ln = left_keys.size();
  const std::size_t rn = right_keys.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t i = 0, j = 0;
    
    while (i < ln && j < rn) {
      uint32_t lkey = lk[i];
      uint32_t rkey = rk[j];
      
      if (lkey < rkey) {
        // Take from left
        hash ^= lkey;
        hash *= 1099511628211ULL;
        hash ^= lv[i];
        hash *= 1099511628211ULL;
        ++i;
      } else if (rkey < lkey) {
        // Take from right
        hash ^= rkey;
        hash *= 1099511628211ULL;
        hash ^= rv[j];
        hash *= 1099511628211ULL;
        ++j;
      } else {
        // Equal keys: keep-last means right wins, skip left
        hash ^= rkey;
        hash *= 1099511628211ULL;
        hash ^= rv[j];
        hash *= 1099511628211ULL;
        ++i;
        ++j;
      }
    }
    
    // Drain remaining left
    while (i < ln) {
      hash ^= lk[i];
      hash *= 1099511628211ULL;
      hash ^= lv[i];
      hash *= 1099511628211ULL;
      ++i;
    }
    
    // Drain remaining right
    while (j < rn) {
      hash ^= rk[j];
      hash *= 1099511628211ULL;
      hash ^= rv[j];
      hash *= 1099511628211ULL;
      ++j;
    }
  }
  
  return hash;
}