#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* base = &logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts)];
      // Initialize best0 and best1 with first two experts, ordered by comparator
      int32_t best0_val, best1_val;
      int best0_idx, best1_idx;
      int32_t v0 = base[0];
      int32_t v1 = base[1];
      if (v0 > v1 || (v0 == v1 && 0 < 1)) {
        best0_val = v0; best0_idx = 0;
        best1_val = v1; best1_idx = 1;
      } else {
        best0_val = v1; best0_idx = 1;
        best1_val = v0; best1_idx = 0;
      }
      for (int e = 2; e < experts; ++e) {
        int32_t val = base[e];
        if (val > best0_val || (val == best0_val && e < best0_idx)) {
          best1_val = best0_val; best1_idx = best0_idx;
          best0_val = val; best0_idx = e;
        } else if (val > best1_val || (val == best1_val && e < best1_idx)) {
          best1_val = val; best1_idx = e;
        }
      }
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(best0_val));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(best0_idx);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(best1_val));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(best1_idx);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
