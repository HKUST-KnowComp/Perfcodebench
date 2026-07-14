#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  if (tokens <= 0 || experts <= 0 || iters <= 0) return hash;

  const int32_t* data = logits.data();
  const std::size_t experts_sz = static_cast<std::size_t>(experts);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    const int32_t* row = data;
    for (int t = 0; t < tokens; ++t, row += experts_sz) {
      int best1_idx = 0;
      int best2_idx = experts > 1 ? 1 : 0;
      int32_t best1_val = row[best1_idx];
      int32_t best2_val = row[best2_idx];

      if (experts > 1) {
        if (best2_val > best1_val || (best2_val == best1_val && best2_idx < best1_idx)) {
          const int tmp_idx = best1_idx;
          best1_idx = best2_idx;
          best2_idx = tmp_idx;
          const int32_t tmp_val = best1_val;
          best1_val = best2_val;
          best2_val = tmp_val;
        }

        for (int e = 2; e < experts; ++e) {
          const int32_t v = row[e];
          if (v > best1_val || (v == best1_val && e < best1_idx)) {
            best2_val = best1_val;
            best2_idx = best1_idx;
            best1_val = v;
            best1_idx = e;
          } else if (v > best2_val || (v == best2_val && e < best2_idx)) {
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
