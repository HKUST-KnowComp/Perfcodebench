#include "interface.h"

namespace {

static inline bool better_pair(int32_t v1, int i1, int32_t v2, int i2) {
  return (v1 > v2) || (v1 == v2 && i1 < i2);
}

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;

  const int32_t* base = logits.data();
  const std::size_t stride = static_cast<std::size_t>(experts);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = base + static_cast<std::size_t>(t) * stride;

      // Initialize best two: values to smallest possible, indices to large sentinel
      int32_t best1_val = INT32_MIN;
      int best1_idx = experts;  // sentinel large index for tie-breaking
      int32_t best2_val = INT32_MIN;
      int best2_idx = experts;

      for (int e = 0; e < experts; ++e) {
        int32_t v = row[e];
        if (better_pair(v, e, best1_val, best1_idx)) {
          // Promote current best1 to best2
          best2_val = best1_val;
          best2_idx = best1_idx;
          best1_val = v;
          best1_idx = e;
        } else {
          // Consider for best2
          if (better_pair(v, e, best2_val, best2_idx)) {
            best2_val = v;
            best2_idx = e;
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
