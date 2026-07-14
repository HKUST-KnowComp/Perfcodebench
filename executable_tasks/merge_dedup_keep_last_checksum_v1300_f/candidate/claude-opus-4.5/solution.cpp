#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    const uint32_t* lk = left_keys.data();
    const uint32_t* lv = left_values.data();
    const uint32_t* rk = right_keys.data();
    const uint32_t* rv = right_values.data();
    
    size_t li = 0, ri = 0;
    const size_t ln = left_keys.size();
    const size_t rn = right_keys.size();
    
    // Two-pointer merge with keep-last semantics
    // Both inputs are sorted. On duplicate keys, right side wins.
    while (li < ln && ri < rn) {
      uint32_t lkey = lk[li];
      uint32_t rkey = rk[ri];
      
      if (lkey < rkey) {
        // Output left element
        hash ^= lkey;
        hash *= 1099511628211ULL;
        hash ^= lv[li];
        hash *= 1099511628211ULL;
        ++li;
      } else if (rkey < lkey) {
        // Output right element
        hash ^= rkey;
        hash *= 1099511628211ULL;
        hash ^= rv[ri];
        hash *= 1099511628211ULL;
        ++ri;
      } else {
        // Keys equal - keep right (last), skip left
        hash ^= rkey;
        hash *= 1099511628211ULL;
        hash ^= rv[ri];
        hash *= 1099511628211ULL;
        ++li;
        ++ri;
      }
    }
    
    // Drain remaining left elements
    while (li < ln) {
      hash ^= lk[li];
      hash *= 1099511628211ULL;
      hash ^= lv[li];
      hash *= 1099511628211ULL;
      ++li;
    }
    
    // Drain remaining right elements
    while (ri < rn) {
      hash ^= rk[ri];
      hash *= 1099511628211ULL;
      hash ^= rv[ri];
      hash *= 1099511628211ULL;
      ++ri;
    }
  }
  
  return hash;
}