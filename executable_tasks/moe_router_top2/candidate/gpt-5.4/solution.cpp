#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline bool better_pair(int32_t va, int ia, int32_t vb, int ib) {
  return (va > vb) || (va == vb && ia < ib);
}

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;

  if (tokens <= 0 || iters <= 0) {
    return hash;
  }

  const std::size_t experts_sz = static_cast<std::size_t>(experts);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = logits.data() + static_cast<std::size_t>(t) * experts_sz;

      int best1_idx = 0;
      int best2_idx = 1;
      int32_t best1_val = row[0];
      int32_t best2_val = row[1];

      if (better_pair(best2_val, best2_idx, best1_val, best1_idx)) {
        const int tmp_idx = best1_idx;
        best1_idx = best2_idx;
        best2_idx = tmp_idx;
        const int32_t tmp_val = best1_val;
        best1_val = best2_val;
        best2_val = tmp_val;
      }

      for (int e = 2; e < experts; ++e) {
        const int32_t v = row[e];
        if (better_pair(v, e, best1_val, best1_idx)) {
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
    }
  }

  return hash;
}
