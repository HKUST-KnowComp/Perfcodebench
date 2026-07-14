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

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      
      // Find top-2 with tie-breaking: higher logit wins, on tie lower index wins
      int32_t top1_val, top2_val;
      int top1_idx, top2_idx;
      
      // Initialize with first two elements in correct order
      if (row[0] > row[1] || (row[0] == row[1] && 0 < 1)) {
        top1_val = row[0]; top1_idx = 0;
        top2_val = row[1]; top2_idx = 1;
      } else {
        top1_val = row[1]; top1_idx = 1;
        top2_val = row[0]; top2_idx = 0;
      }
      
      // Scan remaining elements
      for (int e = 2; e < experts; ++e) {
        int32_t val = row[e];
        
        // Check if better than top1
        if (val > top1_val || (val == top1_val && e < top1_idx)) {
          // Current top1 becomes top2
          top2_val = top1_val;
          top2_idx = top1_idx;
          // New top1
          top1_val = val;
          top1_idx = e;
        }
        // Check if better than top2 (but not top1)
        else if (val > top2_val || (val == top2_val && e < top2_idx)) {
          top2_val = val;
          top2_idx = e;
        }
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_val)));
      hash = mix(hash, static_cast<uint64_t>(top1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_val)));
      hash = mix(hash, static_cast<uint64_t>(top2_idx));
    }
  }
  
  return hash;
}