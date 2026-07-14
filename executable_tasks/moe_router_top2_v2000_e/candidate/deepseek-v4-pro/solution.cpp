#include "interface.h"

#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
      const int32_t* row = logits.data() + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);

      // initialize best-1 and best-2 with the first two experts
      int best1_idx = 0;
      int32_t best1_val = row[0];
      int best2_idx = 1;
      int32_t best2_val = row[1];

      // ensure best1 is strictly better than best2 (higher logit, tie-break by lower index)
      if (best1_val < best2_val || (best1_val == best2_val && best1_idx > best2_idx)) {
        std::swap(best1_val, best2_val);
        std::swap(best1_idx, best2_idx);
      }

      // linear scan of remaining experts
      for (int e = 2; e < experts; ++e) {
        const int32_t v = row[e];
        const int idx = e;
        if (v > best1_val || (v == best1_val && idx < best1_idx)) {
          best2_val = best1_val;
          best2_idx = best1_idx;
          best1_val = v;
          best1_idx = idx;
        } else if (v > best2_val || (v == best2_val && idx < best2_idx)) {
          best2_val = v;
          best2_idx = idx;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_val)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_val)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));
    }
  }
  return hash;
}
