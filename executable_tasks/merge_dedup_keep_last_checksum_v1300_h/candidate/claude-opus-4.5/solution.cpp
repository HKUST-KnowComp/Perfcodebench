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
  
  const std::size_t ln = left_keys.size();
  const std::size_t rn = right_keys.size();
  
  // Pre-allocate output buffers once
  std::vector<uint32_t> out_keys;
  std::vector<uint32_t> out_values;
  out_keys.reserve(ln + rn);
  out_values.reserve(ln + rn);
  
  const uint32_t* lk = ln > 0 ? left_keys.data() : nullptr;
  const uint32_t* lv = ln > 0 ? left_values.data() : nullptr;
  const uint32_t* rk = rn > 0 ? right_keys.data() : nullptr;
  const uint32_t* rv = rn > 0 ? right_values.data() : nullptr;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out_keys.clear();
    out_values.clear();
    
    std::size_t li = 0;
    std::size_t ri = 0;
    
    // Two-pointer merge with keep-last deduplication
    while (li < ln && ri < rn) {
      uint32_t lkey = lk[li];
      uint32_t rkey = rk[ri];
      
      if (lkey < rkey) {
        // Left key is smaller, add it (if not duplicate of last)
        if (out_keys.empty() || out_keys.back() != lkey) {
          out_keys.push_back(lkey);
          out_values.push_back(lv[li]);
        } else {
          // Duplicate - keep last (which is this left value)
          out_values.back() = lv[li];
        }
        ++li;
      } else if (rkey < lkey) {
        // Right key is smaller, add it (if not duplicate of last)
        if (out_keys.empty() || out_keys.back() != rkey) {
          out_keys.push_back(rkey);
          out_values.push_back(rv[ri]);
        } else {
          // Duplicate - keep last (which is this right value)
          out_values.back() = rv[ri];
        }
        ++ri;
      } else {
        // Keys are equal - right takes precedence (keep-last with right after left)
        if (out_keys.empty() || out_keys.back() != rkey) {
          out_keys.push_back(rkey);
          out_values.push_back(rv[ri]);
        } else {
          // Duplicate of previous output - update with right value
          out_values.back() = rv[ri];
        }
        ++li;
        ++ri;
      }
    }
    
    // Drain remaining left
    while (li < ln) {
      uint32_t lkey = lk[li];
      if (out_keys.empty() || out_keys.back() != lkey) {
        out_keys.push_back(lkey);
        out_values.push_back(lv[li]);
      } else {
        out_values.back() = lv[li];
      }
      ++li;
    }
    
    // Drain remaining right
    while (ri < rn) {
      uint32_t rkey = rk[ri];
      if (out_keys.empty() || out_keys.back() != rkey) {
        out_keys.push_back(rkey);
        out_values.push_back(rv[ri]);
      } else {
        out_values.back() = rv[ri];
      }
      ++ri;
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    const std::size_t out_size = out_keys.size();
    const uint32_t* ok = out_keys.data();
    const uint32_t* ov = out_values.data();
    for (std::size_t i = 0; i < out_size; ++i) {
      hash = mix(hash, ok[i]);
      hash = mix(hash, ov[i]);
    }
  }
  
  return hash;
}