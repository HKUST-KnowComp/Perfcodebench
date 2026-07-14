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
  
  // Pre-allocate output vector outside the loop
  std::vector<std::pair<uint32_t, uint32_t>> out;
  out.reserve(ln + rn);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    
    size_t li = 0;
    size_t ri = 0;
    
    // Two-pointer merge of sorted arrays
    // When keys are equal, we process left first, then right overwrites (keep-last)
    // But we need to handle duplicates within each array too
    while (li < ln && ri < rn) {
      uint32_t lk = left_keys[li];
      uint32_t rk = right_keys[ri];
      
      if (lk < rk) {
        // Left key is smaller, add it (or update if duplicate)
        if (!out.empty() && out.back().first == lk) {
          out.back().second = left_values[li];
        } else {
          out.emplace_back(lk, left_values[li]);
        }
        ++li;
      } else if (rk < lk) {
        // Right key is smaller, add it (or update if duplicate)
        if (!out.empty() && out.back().first == rk) {
          out.back().second = right_values[ri];
        } else {
          out.emplace_back(rk, right_values[ri]);
        }
        ++ri;
      } else {
        // Keys are equal - stable_sort keeps left before right, so process left first
        // then right will overwrite (keep-last semantics)
        if (!out.empty() && out.back().first == lk) {
          out.back().second = left_values[li];
        } else {
          out.emplace_back(lk, left_values[li]);
        }
        ++li;
        // Now process right - it will overwrite since same key
        out.back().second = right_values[ri];
        ++ri;
      }
    }
    
    // Process remaining left elements
    while (li < ln) {
      uint32_t lk = left_keys[li];
      if (!out.empty() && out.back().first == lk) {
        out.back().second = left_values[li];
      } else {
        out.emplace_back(lk, left_values[li]);
      }
      ++li;
    }
    
    // Process remaining right elements
    while (ri < rn) {
      uint32_t rk = right_keys[ri];
      if (!out.empty() && out.back().first == rk) {
        out.back().second = right_values[ri];
      } else {
        out.emplace_back(rk, right_values[ri]);
      }
      ++ri;
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (const auto& kv : out) {
      hash = mix(hash, kv.first);
      hash = mix(hash, kv.second);
    }
  }
  
  return hash;
}