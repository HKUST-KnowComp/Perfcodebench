#include "interface.h"

#include <climits>
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
  const size_t experts_sz = static_cast<size_t>(experts);
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const size_t base = static_cast<size_t>(t) * experts_sz;
      int32_t first_logit = INT32_MIN;
      int first_idx = -1;
      int32_t second_logit = INT32_MIN;
      int second_idx = -1;

      for (int e = 0; e < experts; ++e) {
        const int32_t curr_logit = logits[base + static_cast<size_t>(e)];
        const int curr_idx = e;

        const bool better_than_first = (curr_logit > first_logit) || 
                                       (curr_logit == first_logit && curr_idx < first_idx);
        if (better_than_first) {
          second_logit = first_logit;
          second_idx = first_idx;
          first_logit = curr_logit;
          first_idx = curr_idx;
        } else {
          const bool better_than_second = (curr_logit > second_logit) || 
                                         (curr_logit == second_logit && curr_idx < second_idx);
          if (better_than_second) {
            second_logit = curr_logit;
            second_idx = curr_idx;
          }
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(first_logit)));
      hash = mix(hash, static_cast<uint64_t>(first_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_logit)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }
  return hash;
}