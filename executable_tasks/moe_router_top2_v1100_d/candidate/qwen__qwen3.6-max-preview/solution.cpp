#include "interface.h"

#include <cstdint>
#include <vector>
#include <climits>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
    const int32_t* row_ptr = data;
    for (int t = 0; t < tokens; ++t) {
      int32_t top1_val = INT32_MIN;
      int top1_idx = experts;
      int32_t top2_val = INT32_MIN;
      int top2_idx = experts;
      
      for (int e = 0; e < experts; ++e) {
        int32_t val = row_ptr[e];
        if (val > top1_val || (val == top1_val && e < top1_idx)) {
          top2_val = top1_val;
          top2_idx = top1_idx;
          top1_val = val;
          top1_idx = e;
        } else if (val > top2_val || (val == top2_val && e < top2_idx)) {
          top2_val = val;
          top2_idx = e;
        }
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_val)));
      hash = mix(hash, static_cast<uint64_t>(top1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_val)));
      hash = mix(hash, static_cast<uint64_t>(top2_idx));
      
      row_ptr += experts;
    }
  }
  return hash;
}
