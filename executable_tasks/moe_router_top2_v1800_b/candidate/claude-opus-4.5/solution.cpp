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
      // top1: best (highest logit, lowest index on tie)
      // top2: second best
      
      int32_t val1 = row[0];
      int idx1 = 0;
      int32_t val2 = std::numeric_limits<int32_t>::min();
      int idx2 = -1;
      
      for (int e = 1; e < experts; ++e) {
        int32_t v = row[e];
        // Check if this is better than top1
        // better means: higher value, or same value with lower index
        // Since we iterate in order, same value won't beat top1 (top1 has lower index)
        if (v > val1) {
          // Current top1 becomes top2
          val2 = val1;
          idx2 = idx1;
          // New top1
          val1 = v;
          idx1 = e;
        } else if (idx2 < 0) {
          // No top2 yet
          val2 = v;
          idx2 = e;
        } else if (v > val2 || (v == val2 && e < idx2)) {
          // Better than current top2
          val2 = v;
          idx2 = e;
        }
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(val1)));
      hash = mix(hash, static_cast<uint64_t>(idx1));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(val2)));
      hash = mix(hash, static_cast<uint64_t>(idx2));
    }
  }
  return hash;
}