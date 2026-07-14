#include "interface.h"

#include <cstdint>
#include <vector>

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();
  const size_t stokens = static_cast<size_t>(tokens);
  const size_t sexperts = static_cast<size_t>(experts);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis
    const int32_t* row = data;

    for (size_t t = 0; t < stokens; ++t) {
      // Assume at least two experts as in the baseline harness.
      // Initialize top-2 with first two experts, then scan the rest.
      int best1_idx = 0;
      int best2_idx = 1;
      int32_t best1_val = row[0];
      int32_t best2_val = row[1];

      // Ensure best1 is better than best2 with the same comparator used by baseline.
      if (best2_val > best1_val || (best2_val == best1_val && best2_idx < best1_idx)) {
        int32_t tmpv = best1_val; best1_val = best2_val; best2_val = tmpv;
        int tmpi = best1_idx; best1_idx = best2_idx; best2_idx = tmpi;
      }

      for (size_t e = 2; e < sexperts; ++e) {
        int32_t v = row[e];
        int ei = static_cast<int>(e);
        if (v > best1_val || (v == best1_val && ei < best1_idx)) {
          // Promote to best1, demote previous best1 to best2.
          best2_val = best1_val;
          best2_idx = best1_idx;
          best1_val = v;
          best1_idx = ei;
        } else if (v > best2_val || (v == best2_val && ei < best2_idx)) {
          best2_val = v;
          best2_idx = ei;
        }
      }

      // Mix results into the checksum, matching baseline casting behavior.
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_val)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_val)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));

      row += sexperts;
    }
  }

  return hash;
}
