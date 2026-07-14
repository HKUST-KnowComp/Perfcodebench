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
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = &logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts)];
      // Initialize top two with first two elements
      int best1_idx = 0;
      int best2_idx = 1;
      int32_t best1_val = row[0];
      int32_t best2_val = row[1];
      // Ensure best1 is strictly better than best2
      if (best1_val < best2_val || (best1_val == best2_val && best1_idx > best2_idx)) {
        std::swap(best1_val, best2_val);
        std::swap(best1_idx, best2_idx);
      }
      // Scan remaining experts
      for (int e = 2; e < experts; ++e) {
        int32_t val = row[e];
        // If better than current second best
        if (val > best2_val || (val == best2_val && e < best2_idx)) {
          best2_val = val;
          best2_idx = e;
          // Promote to best1 if it becomes better than best1
          if (best2_val > best1_val || (best2_val == best1_val && best2_idx < best1_idx)) {
            std::swap(best1_val, best2_val);
            std::swap(best1_idx, best2_idx);
          }
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