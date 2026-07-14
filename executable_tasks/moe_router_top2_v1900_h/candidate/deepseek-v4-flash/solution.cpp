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
  const int32_t* data = logits.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      // Linear scan to find top-2 experts
      int best1_logit = INT32_MIN;
      int best1_idx = INT_MAX;
      int best2_logit = INT32_MIN;
      int best2_idx = INT_MAX;

      const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        int32_t logit = row[e];
        // Compare with best1
        if (logit > best1_logit || (logit == best1_logit && e < best1_idx)) {
          best2_logit = best1_logit;
          best2_idx = best1_idx;
          best1_logit = logit;
          best1_idx = e;
        } else if (logit > best2_logit || (logit == best2_logit && e < best2_idx)) {
          best2_logit = logit;
          best2_idx = e;
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
