#include "interface.h"
#include <climits>
#include <cstdint>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const std::size_t stride = static_cast<std::size_t>(experts);
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const std::size_t base = static_cast<std::size_t>(t) * stride;
      int32_t best_val = INT32_MIN;
      int best_idx = -1;
      int32_t second_val = INT32_MIN;
      int second_idx = -1;
      for (int e = 0; e < experts; ++e) {
        int32_t v = logits[base + static_cast<std::size_t>(e)];
        if (v > best_val || (v == best_val && e < best_idx)) {
          second_val = best_val;
          second_idx = best_idx;
          best_val = v;
          best_idx = e;
        } else if (v > second_val || (v == second_val && e < second_idx)) {
          second_val = v;
          second_idx = e;
        }
      }
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(best_val));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(best_idx);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(second_val));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(second_idx);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
