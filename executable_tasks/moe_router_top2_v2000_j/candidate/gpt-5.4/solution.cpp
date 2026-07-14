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

  const std::size_t experts_sz = static_cast<std::size_t>(experts);
  const int32_t* data = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    const int32_t* row = data;
    for (int t = 0; t < tokens; ++t, row += experts_sz) {
      int best1_idx = 0;
      int32_t best1_val = row[0];

      int best2_idx;
      int32_t best2_val;

      if (experts >= 2) {
        const int32_t v1 = row[1];
        if (v1 > best1_val || (v1 == best1_val && 1 < best1_idx)) {
          best2_idx = best1_idx;
          best2_val = best1_val;
          best1_idx = 1;
          best1_val = v1;
        } else {
          best2_idx = 1;
          best2_val = v1;
        }

        for (int e = 2; e < experts; ++e) {
          const int32_t v = row[e];
          if (v > best1_val || (v == best1_val && e < best1_idx)) {
            best2_idx = best1_idx;
            best2_val = best1_val;
            best1_idx = e;
            best1_val = v;
          } else if (v > best2_val || (v == best2_val && e < best2_idx)) {
            best2_idx = e;
            best2_val = v;
          }
        }
      } else {
        best2_idx = 0;
        best2_val = row[0];
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_val)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_val)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));
    }
  }

  return hash;
}
