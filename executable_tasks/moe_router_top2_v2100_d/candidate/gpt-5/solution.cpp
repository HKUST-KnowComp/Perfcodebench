#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* base = logits.data();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV offset basis
    const int32_t* p = base;

    for (int t = 0; t < tokens; ++t) {
      // Assume experts >= 2 as per task semantics
      // Initialize top-2 using first two experts with correct tie-breaking
      int32_t v0 = p[0];
      int32_t v1 = p[1];

      int best1_idx;
      int best2_idx;
      int32_t best1_val;
      int32_t best2_val;

      if (v1 > v0) {
        best1_val = v1; best1_idx = 1;
        best2_val = v0; best2_idx = 0;
      } else { // v1 <= v0; ties prefer lower index (0)
        best1_val = v0; best1_idx = 0;
        best2_val = v1; best2_idx = 1;
      }

      for (int e = 2; e < experts; ++e) {
        int32_t v = p[e];
        if (v > best1_val || (v == best1_val && e < best1_idx)) {
          // Promote to best1, demote previous best1 to best2
          best2_val = best1_val; best2_idx = best1_idx;
          best1_val = v; best1_idx = e;
        } else if (v > best2_val || (v == best2_val && e < best2_idx)) {
          best2_val = v; best2_idx = e;
        }
      }

      // Mix results exactly as baseline (value cast to uint32_t -> uint64_t)
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_val)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_val)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));

      p += experts;
    }
  }

  return hash;
}
