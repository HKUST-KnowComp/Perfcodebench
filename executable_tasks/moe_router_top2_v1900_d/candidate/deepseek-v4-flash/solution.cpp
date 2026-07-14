#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

bool better(int32_t logit_a, int idx_a, int32_t logit_b, int idx_b) {
  return logit_a > logit_b || (logit_a == logit_b && idx_a < idx_b);
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* data = logits.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      std::size_t base = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      // Initialize best and second with the first expert
      int32_t best_logit = data[base];
      int best_idx = 0;
      int32_t second_logit = INT32_MIN;
      int second_idx = -1;
      // Scan the remaining experts
      for (int e = 1; e < experts; ++e) {
        int32_t cur_logit = data[base + static_cast<std::size_t>(e)];
        int cur_idx = e;
        if (better(cur_logit, cur_idx, best_logit, best_idx)) {
          second_logit = best_logit;
          second_idx = best_idx;
          best_logit = cur_logit;
          best_idx = cur_idx;
        } else if (better(cur_logit, cur_idx, second_logit, second_idx)) {
          second_logit = cur_logit;
          second_idx = cur_idx;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_logit)));
      hash = mix(hash, static_cast<uint64_t>(best_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_logit)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }
  return hash;
}
