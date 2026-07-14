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
  const std::size_t stride = static_cast<std::size_t>(experts);
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* token_logits = data + static_cast<std::size_t>(t) * stride;
      int32_t best1_logit = token_logits[0];
      int best1_idx = 0;
      int32_t best2_logit = token_logits[1];
      int best2_idx = 1;
      if (best2_logit > best1_logit || (best2_logit == best1_logit && best2_idx < best1_idx)) {
        std::swap(best1_logit, best2_logit);
        std::swap(best1_idx, best2_idx);
      }
      for (int e = 2; e < experts; ++e) {
        int32_t cur_logit = token_logits[e];
        int cur_idx = e;
        if (cur_logit > best1_logit || (cur_logit == best1_logit && cur_idx < best1_idx)) {
          best2_logit = best1_logit;
          best2_idx = best1_idx;
          best1_logit = cur_logit;
          best1_idx = cur_idx;
        } else if (cur_logit > best2_logit || (cur_logit == best2_logit && cur_idx < best2_idx)) {
          best2_logit = cur_logit;
          best2_idx = cur_idx;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_logit)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_logit)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));
    }
  }
  return hash;
}