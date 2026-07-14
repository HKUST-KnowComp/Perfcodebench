#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    const std::size_t experts_st = static_cast<std::size_t>(experts);
    for (int t = 0; t < tokens; ++t) {
      const std::size_t token_base = static_cast<std::size_t>(t) * experts_st;
      // Initialize top-1 with first expert
      int32_t top1_val = logits[token_base + 0];
      int top1_idx = 0;
      int32_t top2_val;
      int top2_idx;
      
      // Process second expert to initialize top-2
      const int32_t e1_val = logits[token_base + 1];
      if (e1_val > top1_val || (e1_val == top1_val && 1 < top1_idx)) {
        top2_val = top1_val;
        top2_idx = top1_idx;
        top1_val = e1_val;
        top1_idx = 1;
      } else {
        top2_val = e1_val;
        top2_idx = 1;
      }
      
      // Scan remaining experts to update top-2
      for (int e = 2; e < experts; ++e) {
        const int32_t curr_val = logits[token_base + static_cast<std::size_t>(e)];
        const int curr_idx = e;
        
        if (curr_val > top1_val || (curr_val == top1_val && curr_idx < top1_idx)) {
          // Shift top1 to top2, update top1
          top2_val = top1_val;
          top2_idx = top1_idx;
          top1_val = curr_val;
          top1_idx = curr_idx;
        } else if (curr_val > top2_val || (curr_val == top2_val && curr_idx < top2_idx)) {
          // Update only top2
          top2_val = curr_val;
          top2_idx = curr_idx;
        }
      }
      
      // Match original hash update order exactly to preserve checksum
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_val)));
      hash = mix(hash, static_cast<uint64_t>(top1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_val)));
      hash = mix(hash, static_cast<uint64_t>(top2_idx));
    }
    final_hash = hash;
  }
  return final_hash;
}