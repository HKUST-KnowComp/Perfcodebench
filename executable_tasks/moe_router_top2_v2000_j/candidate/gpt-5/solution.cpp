#include "interface.h"

#include <cstdint>
#include <vector>
#include <climits>

namespace {

static inline bool better_val_idx(int32_t v1, int idx1, int32_t v2, int idx2) {
  return (v1 > v2) || (v1 == v2 && idx1 < idx2);
}

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV-1a offset basis

    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<size_t>(t) * static_cast<size_t>(experts);

      // Initialize top-1 and top-2 with sentinels ensuring tie-breaking correctness
      int32_t top1_val = INT32_MIN;
      int      top1_idx = INT_MAX;
      int32_t top2_val = INT32_MIN;
      int      top2_idx = INT_MAX;

      for (int e = 0; e < experts; ++e) {
        int32_t v = row[e];
        if (better_val_idx(v, e, top1_val, top1_idx)) {
          // Shift current top1 to top2
          top2_val = top1_val;
          top2_idx = top1_idx;
          // New top1
          top1_val = v;
          top1_idx = e;
        } else if (better_val_idx(v, e, top2_val, top2_idx)) {
          // Update top2
          top2_val = v;
          top2_idx = e;
        }
      }

      // Mix in values and indices exactly as baseline
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_val)));
      hash = mix(hash, static_cast<uint64_t>(top1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_val)));
      hash = mix(hash, static_cast<uint64_t>(top2_idx));
    }
  }

  return hash;
}
