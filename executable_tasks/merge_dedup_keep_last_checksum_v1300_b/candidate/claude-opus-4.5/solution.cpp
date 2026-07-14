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
  
  // Pre-allocate output buffer once outside the loop
  std::vector<uint64_t> merged_kv;
  merged_kv.reserve(ln + rn);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    merged_kv.clear();
    
    // Merge from right to left (descending order)
    // This way we naturally see the "last" occurrence first for duplicates
    // Right side values take precedence on duplicate keys
    
    size_t li = ln;
    size_t ri = rn;
    
    while (li > 0 && ri > 0) {
      uint32_t lk = left_keys[li - 1];
      uint32_t rk = right_keys[ri - 1];
      
      if (lk > rk) {
        // Left key is larger, take it
        merged_kv.push_back((static_cast<uint64_t>(lk) << 32) | left_values[li - 1]);
        --li;
      } else if (rk > lk) {
        // Right key is larger, take it
        merged_kv.push_back((static_cast<uint64_t>(rk) << 32) | right_values[ri - 1]);
        --ri;
      } else {
        // Equal keys - take right value (keep-last semantics)
        merged_kv.push_back((static_cast<uint64_t>(rk) << 32) | right_values[ri - 1]);
        --li;
        --ri;
      }
    }
    
    // Drain remaining left
    while (li > 0) {
      --li;
      merged_kv.push_back((static_cast<uint64_t>(left_keys[li]) << 32) | left_values[li]);
    }
    
    // Drain remaining right
    while (ri > 0) {
      --ri;
      merged_kv.push_back((static_cast<uint64_t>(right_keys[ri]) << 32) | right_values[ri]);
    }
    
    // Compute hash in ascending order (reverse of how we built it)
    hash = 1469598103934665603ULL;
    for (size_t i = merged_kv.size(); i > 0; --i) {
      uint64_t kv = merged_kv[i - 1];
      uint32_t key = static_cast<uint32_t>(kv >> 32);
      uint32_t val = static_cast<uint32_t>(kv);
      hash = mix(hash, key);
      hash = mix(hash, val);
    }
  }
  
  return hash;
}