#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_ptr = &logits[static_cast<size_t>(t) * experts];
      
      int32_t best_val = -2147483648;
      int best_idx = 2147483647;
      int32_t second_val = -2147483648;
      int second_idx = 2147483647;

      for (int e = 0; e < experts; ++e) {
        int32_t val = row_ptr[e];
        if (val > best_val || (val == best_val && e < best_idx)) {
          second_val = best_val;
          second_idx = best_idx;
          best_val = val;
          best_idx = e;
        } else if (val > second_val || (val == second_val && e < second_idx)) {
          second_val = val;
          second_idx = e;
        }
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
      hash = mix(hash, static_cast<uint64_t>(best_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }
  return hash;
}