#include "interface.h"

#include <cstdint>
#include <vector>
#include <limits>

namespace {

// FNV-1a style mixing function as used in the baseline
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  if (tokens <= 0 || experts < 2) return 0;

  const int32_t* logits_ptr = logits.data();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = logits_ptr + (static_cast<size_t>(t) * experts);
      
      int32_t max1_val = std::numeric_limits<int32_t>::min();
      int max1_idx = -1;
      int32_t max2_val = std::numeric_limits<int32_t>::min();
      int max2_idx = -1;

      // Single pass to find top 2 with deterministic tie-breaking (smaller index wins)
      for (int e = 0; e < experts; ++e) {
        int32_t val = row[e];
        if (val > max1_val) {
          max2_val = max1_val;
          max2_idx = max1_idx;
          max1_val = val;
          max1_idx = e;
        } else if (val == max1_val) {
          // Tie-break: smaller index is 'better'. 
          // Since we iterate e from 0 to experts-1, the first occurrence of max1_val 
          // is already in max1_idx. Any subsequent occurrence is a candidate for max2.
          if (max2_idx == -1 || val > max2_val) {
             max2_val = val;
             max2_idx = e;
          }
        } else if (val > max2_val) {
          max2_val = val;
          max2_idx = e;
        }
      }

      // Update hash with top-1 then top-2 results
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max1_val)));
      hash = mix(hash, static_cast<uint64_t>(max1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max2_val)));
      hash = mix(hash, static_cast<uint64_t>(max2_idx));
    }
  }
  
  return hash;
}