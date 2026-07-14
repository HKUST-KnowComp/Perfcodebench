#include "interface.h"

#include <limits>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
    const int32_t* row_ptr = data;

    for (int t = 0; t < tokens; ++t) {
      int32_t best1_val = std::numeric_limits<int32_t>::min();
      int     best1_idx = std::numeric_limits<int>::max();
      int32_t best2_val = std::numeric_limits<int32_t>::min();
      int     best2_idx = std::numeric_limits<int>::max();

      for (int e = 0; e < experts; ++e) {
        int32_t v = row_ptr[e];
        if (v > best1_val || (v == best1_val && e < best1_idx)) {
          // Shift current best1 to best2
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

      row_ptr += experts;
    }
  }

  return hash;
}