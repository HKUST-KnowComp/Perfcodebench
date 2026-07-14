#include "interface.h"

#include <limits>

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
  const int32_t* data = logits.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV offset basis
    const int32_t* p = data;
    for (int t = 0; t < tokens; ++t) {
      int32_t best_val = std::numeric_limits<int32_t>::min();
      int best_idx = std::numeric_limits<int>::max();
      int32_t second_val = std::numeric_limits<int32_t>::min();
      int second_idx = std::numeric_limits<int>::max();

      for (int e = 0; e < experts; ++e) {
        int32_t v = p[e];
        if (v > best_val || (v == best_val && e < best_idx)) {
          // Promote current best to second
          second_val = best_val;
          second_idx = best_idx;
          best_val = v;
          best_idx = e;
        } else if (v > second_val || (v == second_val && e < second_idx)) {
          second_val = v;
          second_idx = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
      hash = mix(hash, static_cast<uint64_t>(best_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));

      p += experts;
    }
  }
  return hash;
}
