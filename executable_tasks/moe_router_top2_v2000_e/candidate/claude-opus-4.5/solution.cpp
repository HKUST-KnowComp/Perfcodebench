#include "interface.h"

#include <cstdint>
#include <vector>
#include <limits>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* base = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = base + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      
      // Find top-2 with deterministic tie-breaking (lower index wins)
      int32_t top1_val = std::numeric_limits<int32_t>::min();
      int top1_idx = 0;
      int32_t top2_val = std::numeric_limits<int32_t>::min();
      int top2_idx = 0;
      
      for (int e = 0; e < experts; ++e) {
        int32_t val = row[e];
        if (val > top1_val || (val == top1_val && e < top1_idx)) {
          // Current becomes new top1, old top1 becomes top2
          top2_val = top1_val;
          top2_idx = top1_idx;
          top1_val = val;
          top1_idx = e;
        } else if (val > top2_val || (val == top2_val && e < top2_idx)) {
          // Current becomes new top2
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