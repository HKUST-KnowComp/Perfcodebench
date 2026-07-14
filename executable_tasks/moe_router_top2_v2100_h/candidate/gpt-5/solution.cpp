#include "interface.h"

namespace {

inline bool better_pair(int32_t val_a, int idx_a, int32_t val_b, int idx_b) {
  return (val_a > val_b) || (val_a == val_b && idx_a < idx_b);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
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

    const int32_t* row = data;
    for (int t = 0; t < tokens; ++t) {
      // Assume experts >= 2 as per baseline behavior
      int best1_idx = 0;
      int32_t best1_val = row[0];
      int best2_idx = 1;
      int32_t best2_val = row[1];

      // Ensure best1 is better than best2 according to tie-breaking rules
      if (!better_pair(best1_val, best1_idx, best2_val, best2_idx)) {
        int tmpi = best1_idx; best1_idx = best2_idx; best2_idx = tmpi;
        int32_t tmpv = best1_val; best1_val = best2_val; best2_val = tmpv;
      }

      for (int e = 2; e < experts; ++e) {
        int32_t v = row[e];
        if (better_pair(v, e, best1_val, best1_idx)) {
          // Shift current best1 to best2
          best2_val = best1_val;
          best2_idx = best1_idx;
          best1_val = v;
          best1_idx = e;
        } else if (better_pair(v, e, best2_val, best2_idx)) {
          best2_val = v;
          best2_idx = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_val)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_val)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));

      row += static_cast<std::size_t>(experts);
    }
  }

  return hash;
}
