#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

static inline bool better_pair(int32_t va, int ia, int32_t vb, int ib) {
  return (va > vb) || (va == vb && ia < ib);
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const std::size_t experts_sz = static_cast<std::size_t>(experts);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    const int32_t* base = logits.data();
    for (int t = 0; t < tokens; ++t, base += experts_sz) {
      int best_idx = 0;
      int second_idx = 1;
      int32_t best_val = base[0];
      int32_t second_val = base[1];

      if (better_pair(second_val, second_idx, best_val, best_idx)) {
        const int tmp_idx = best_idx;
        best_idx = second_idx;
        second_idx = tmp_idx;
        const int32_t tmp_val = best_val;
        best_val = second_val;
        second_val = tmp_val;
      }

      for (int e = 2; e < experts; ++e) {
        const int32_t v = base[e];
        if (better_pair(v, e, best_val, best_idx)) {
          second_val = best_val;
          second_idx = best_idx;
          best_val = v;
          best_idx = e;
        } else if (better_pair(v, e, second_val, second_idx)) {
          second_val = v;
          second_idx = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
      hash = mix(hash, static_cast<uint64_t>(best_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }

  return hash;
}
