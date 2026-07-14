#include "interface.h"

#include <vector>
#include <cstdint>

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
  
  const size_t ln = left_keys.size();
  const size_t rn = right_keys.size();
  
  const uint32_t* lk = ln > 0 ? left_keys.data() : nullptr;
  const uint32_t* lv = ln > 0 ? left_values.data() : nullptr;
  const uint32_t* rk = rn > 0 ? right_keys.data() : nullptr;
  const uint32_t* rv = rn > 0 ? right_values.data() : nullptr;
  
  // Pre-allocate output buffer
  std::vector<uint32_t> out_keys;
  std::vector<uint32_t> out_vals;
  out_keys.reserve(ln + rn);
  out_vals.reserve(ln + rn);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out_keys.clear();
    out_vals.clear();
    
    size_t i = 0, j = 0;
    
    // Merge with keep-last semantics:
    // When keys are equal, right side wins, so we take right's value
    while (i < ln && j < rn) {
      uint32_t lkey = lk[i];
      uint32_t rkey = rk[j];
      
      if (lkey < rkey) {
        out_keys.push_back(lkey);
        out_vals.push_back(lv[i]);
        ++i;
      } else if (rkey < lkey) {
        out_keys.push_back(rkey);
        out_vals.push_back(rv[j]);
        ++j;
      } else {
        // Equal keys: right wins (keep last)
        out_keys.push_back(rkey);
        out_vals.push_back(rv[j]);
        ++i;
        ++j;
      }
    }
    
    // Remaining left elements
    while (i < ln) {
      out_keys.push_back(lk[i]);
      out_vals.push_back(lv[i]);
      ++i;
    }
    
    // Remaining right elements
    while (j < rn) {
      out_keys.push_back(rk[j]);
      out_vals.push_back(rv[j]);
      ++j;
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const size_t out_size = out_keys.size();
    const uint32_t* ok = out_keys.data();
    const uint32_t* ov = out_vals.data();
    for (size_t k = 0; k < out_size; ++k) {
      hash = mix(hash, ok[k]);
      hash = mix(hash, ov[k]);
    }
  }
  
  return hash;
}