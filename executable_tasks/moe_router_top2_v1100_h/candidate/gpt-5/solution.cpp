#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* base_ptr = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV offset basis
    const int32_t* p = base_ptr;

    for (int t = 0; t < tokens; ++t) {
      // Assume experts >= 2 as in baseline behavior
      const int32_t* row = p;

      // Initialize top-2 with first two experts
      int best_idx = 0;
      int32_t best_val = row[0];
      int second_idx = 1;
      int32_t second_val = row[1];

      // Ensure best is better than second with tie-breaker
      if (second_val > best_val || (second_val == best_val && second_idx < best_idx)) {
        // swap
        int32_t tv = best_val; best_val = second_val; second_val = tv;
        int ti = best_idx; best_idx = second_idx; second_idx = ti;
      }

      // Scan remaining experts
      for (int e = 2; e < experts; ++e) {
        int32_t v = row[e];
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

      // Mix results into hash, preserving casting behavior of baseline
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
      hash = mix(hash, static_cast<uint64_t>(best_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));

      p += experts;
    }
  }

  return hash;
}
