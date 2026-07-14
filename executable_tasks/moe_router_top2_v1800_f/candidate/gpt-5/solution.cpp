#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  // Preserve baseline behavior for iters==0
  if (iters <= 0) return 0ULL;

  // Assume experts >= 2 per task definition (top-2 routing)
  const int32_t* base = logits.data();
  const size_t experts_sz = static_cast<size_t>(experts);

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  for (int t = 0; t < tokens; ++t) {
    const int32_t* row = base + static_cast<size_t>(t) * experts_sz;

    // Initialize with first two experts ensuring correct order (best then second)
    int best_idx = 0;
    int second_idx = 1;
    int32_t best_val = row[0];
    int32_t second_val = row[1];

    if (second_val > best_val || (second_val == best_val && second_idx < best_idx)) {
      // swap
      int tmp_i = best_idx; best_idx = second_idx; second_idx = tmp_i;
      int32_t tmp_v = best_val; best_val = second_val; second_val = tmp_v;
    }

    // Scan remaining experts to maintain top-2 with deterministic tie-breaking
    for (int e = 2; e < experts; ++e) {
      int32_t v = row[e];
      if (v > best_val || (v == best_val && e < best_idx)) {
        // Promote to best, demote previous best to second
        second_val = best_val;
        second_idx = best_idx;
        best_val = v;
        best_idx = e;
      } else if (v > second_val || (v == second_val && e < second_idx)) {
        second_val = v;
        second_idx = e;
      }
    }

    // Mix in top-2 (value as uint32_t bit pattern, then index), in order
    hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
    hash = mix(hash, static_cast<uint64_t>(best_idx));
    hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
    hash = mix(hash, static_cast<uint64_t>(second_idx));
  }

  return hash;
}
