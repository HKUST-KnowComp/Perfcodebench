#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
    hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<std::size_t>(t) * stride;

      // Initialize best and second-best from the first two experts
      int best_i = 0;
      int32_t best_v = row[0];
      int second_i = 1;
      int32_t second_v = row[1];

      // Ensure (best) is the better of the first two using the same tie-breaking
      if (!(best_v > second_v || (best_v == second_v && best_i < second_i))) {
        // swap best and second
        int tmp_i = best_i; best_i = second_i; second_i = tmp_i;
        int32_t tmp_v = best_v; best_v = second_v; second_v = tmp_v;
      }

      for (int e = 2; e < experts; ++e) {
        int32_t v = row[e];
        // if candidate better than best
        if (v > best_v || (v == best_v && e < best_i)) {
          // demote best to second
          second_v = best_v;
          second_i = best_i;
          best_v = v;
          best_i = e;
        } else if (v > second_v || (v == second_v && e < second_i)) {
          // candidate better than current second (but not better than best)
          second_v = v;
          second_i = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_v)));
      hash = mix(hash, static_cast<uint64_t>(best_i));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_v)));
      hash = mix(hash, static_cast<uint64_t>(second_i));
    }
  }

  return hash;
}
