#include "interface.h"

#include <cstdint>
#include <climits>
#include <utility>

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
    const std::size_t logits_stride = static_cast<std::size_t>(experts);
    for (int t = 0; t < tokens; ++t) {
      const std::size_t token_base = static_cast<std::size_t>(t) * logits_stride;
      int32_t first_val = INT32_MIN;
      int first_idx = 0;
      int32_t second_val = INT32_MIN;
      int second_idx = 0;

      for (int e = 0; e < experts; ++e) {
        const int32_t curr_val = logits[token_base + static_cast<std::size_t>(e)];
        const int curr_idx = e;
        // Exact same ordering predicate as original better() function
        const bool curr_beats_first = (curr_val > first_val) || (curr_val == first_val && curr_idx < first_idx);
        if (curr_beats_first) {
          // Demote current first to second
          second_val = first_val;
          second_idx = first_idx;
          first_val = curr_val;
          first_idx = curr_idx;
        } else {
          const bool curr_beats_second = (curr_val > second_val) || (curr_val == second_val && curr_idx < second_idx);
          if (curr_beats_second) {
            second_val = curr_val;
            second_idx = curr_idx;
          }
        }
      }
      // Same hash mixing as original to guarantee identical checksum
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(first_val)));
      hash = mix(hash, static_cast<uint64_t>(first_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }
  return hash;
}