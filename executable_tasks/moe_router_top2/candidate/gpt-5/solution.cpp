#include "interface.h"

#include <cstdint>
#include <vector>
#include <limits>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* base = logits.data();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV-1a offset basis
    const int32_t* p = base;

    for (int t = 0; t < tokens; ++t) {
      int best1_idx = experts;
      int best2_idx = experts;
      int32_t best1_val = std::numeric_limits<int32_t>::min();
      int32_t best2_val = std::numeric_limits<int32_t>::min();

      for (int e = 0; e < experts; ++e) {
        int32_t v = p[e];
        if (v > best1_val || (v == best1_val && e < best1_idx)) {
          // Demote current best1 to best2
          best2_val = best1_val;
          best2_idx = best1_idx;
          best1_val = v;
          best1_idx = e;
        } else if (v > best2_val || (v == best2_val && e < best2_idx)) {
          best2_val = v;
          best2_idx = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_val)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_val)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));

      p += experts;
    }
  }

  return hash;
}
