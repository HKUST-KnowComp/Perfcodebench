#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = logits.data() + static_cast<std::size_t>(t) * experts;
      int32_t top1_logit = INT32_MIN;
      int32_t top2_logit = INT32_MIN;
      int top1_idx = 0;
      int top2_idx = 0;
      for (int e = 0; e < experts; ++e) {
        int32_t val = row[e];
        if (val > top1_logit || (val == top1_logit && e < top1_idx)) {
          top2_logit = top1_logit;
          top2_idx = top1_idx;
          top1_logit = val;
          top1_idx = e;
        } else if (val > top2_logit || (val == top2_logit && e < top2_idx)) {
          top2_logit = val;
          top2_idx = e;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_logit)));
      hash = mix(hash, static_cast<uint64_t>(top1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_logit)));
      hash = mix(hash, static_cast<uint64_t>(top2_idx));
    }
  }
  return hash;
}
