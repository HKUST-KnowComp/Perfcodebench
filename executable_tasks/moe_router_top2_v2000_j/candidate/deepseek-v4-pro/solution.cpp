#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      int32_t best_val = INT32_MIN;
      int32_t second_val = INT32_MIN;
      int best_idx = -1;
      int second_idx = -1;
      for (int e = 0; e < experts; ++e) {
        int32_t val = row[e];
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
