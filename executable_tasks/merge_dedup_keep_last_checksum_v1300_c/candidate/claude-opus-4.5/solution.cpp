#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

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
    
    std::size_t i = 0;
    std::size_t j = 0;
    
    while (i < ln && j < rn) {
      uint32_t lkey = lk[i];
      uint32_t rkey = rk[j];
      
      if (lkey < rkey) {
        // Left key is smaller, output it
        hash = mix(hash, lkey);
        hash = mix(hash, lv[i]);
        ++i;
      } else if (rkey < lkey) {
        // Right key is smaller, output it
        hash = mix(hash, rkey);
        hash = mix(hash, rv[j]);
        ++j;
      } else {
        // Keys are equal - keep last (right side value)
        hash = mix(hash, lkey);
        hash = mix(hash, rv[j]);
        ++i;
        ++j;
      }
    }
    
    // Drain remaining left
    while (i < ln) {
      hash = mix(hash, lk[i]);
      hash = mix(hash, lv[i]);
      ++i;
    }
    
    // Drain remaining right
    while (j < rn) {
      hash = mix(hash, rk[j]);
      hash = mix(hash, rv[j]);
      ++j;
    }
  }
  
  return hash;
}