#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const int32_t* ptr = data;
    for (int t = 0; t < tokens; ++t) {
      int32_t top1_val = ptr[0];
      int top1_idx = 0;
      int32_t top2_val = ptr[1];
      int top2_idx = 1;
      
      if (top2_val > top1_val) {
        int32_t tmp_v = top1_val; top1_val = top2_val; top2_val = tmp_v;
        int tmp_i = top1_idx; top1_idx = top2_idx; top2_idx = tmp_i;
      }
      
      for (int e = 2; e < experts; ++e) {
        int32_t val = ptr[e];
        if (val > top1_val) {
          top2_val = top1_val;
          top2_idx = top1_idx;
          top1_val = val;
          top1_idx = e;
        } else if (val > top2_val) {
          top2_val = val;
          top2_idx = e;
        }
      }
      
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(top1_val));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(top1_idx);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(top2_val));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(top2_idx);
      hash *= 1099511628211ULL;
      
      ptr += experts;
    }
  }
  return hash;
}
