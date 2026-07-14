#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Returns true if (val_a, idx_a) is strictly better than (val_b, idx_b)
// Higher value wins; on tie, lower index wins
inline bool is_better(int32_t val_a, int idx_a, int32_t val_b, int idx_b) {
  return val_a > val_b || (val_a == val_b && idx_a < idx_b);
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      
      // Find top-2 with deterministic tie-breaking
      int32_t top1_val, top2_val;
      int top1_idx, top2_idx;
      
      // Initialize with first two elements in correct order
      if (experts >= 2) {
        if (is_better(row[0], 0, row[1], 1)) {
          top1_val = row[0]; top1_idx = 0;
          top2_val = row[1]; top2_idx = 1;
        } else {
          top1_val = row[1]; top1_idx = 1;
          top2_val = row[0]; top2_idx = 0;
        }
      } else {
        // Edge case: only 1 expert (shouldn't happen for top-2, but handle it)
        top1_val = row[0]; top1_idx = 0;
        top2_val = row[0]; top2_idx = 0;
      }
      
      // Scan remaining experts
      for (int e = 2; e < experts; ++e) {
        int32_t val = row[e];
        if (is_better(val, e, top1_val, top1_idx)) {
          // New top1, old top1 becomes top2
          top2_val = top1_val;
          top2_idx = top1_idx;
          top1_val = val;
          top1_idx = e;
        } else if (is_better(val, e, top2_val, top2_idx)) {
          // New top2
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