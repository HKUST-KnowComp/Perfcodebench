#include "interface.h"

#include <cstdint>
#include <vector>
#include <climits>

namespace {
bool is_better(int32_t val_a, int idx_a, int32_t val_b, int idx_b) {
  return val_a > val_b || (val_a == val_b && idx_a < idx_b);
}

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
    for (int t = 0; t < tokens; ++t) {
      int32_t first_val = INT32_MIN;
      int first_idx = -1;
      int32_t second_val = INT32_MIN;
      int second_idx = -1;

      const std::size_t token_offset = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        const std::size_t logit_idx = token_offset + static_cast<std::size_t>(e);
        const int32_t curr_val = logits[logit_idx];
        const int curr_idx = e;

        if (is_better(curr_val, curr_idx, first_val, first_idx)) {
          second_val = first_val;
          second_idx = first_idx;
          first_val = curr_val;
          first_idx = curr_idx;
        } else if (is_better(curr_val, curr_idx, second_val, second_idx)) {
          second_val = curr_val;
          second_idx = curr_idx;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(first_val)));
      hash = mix(hash, static_cast<uint64_t>(first_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
    final_hash = hash;
  }
  return final_hash;
}