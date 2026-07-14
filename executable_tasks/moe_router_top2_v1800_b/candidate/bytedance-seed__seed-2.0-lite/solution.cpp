#include "interface.h"
#include <climits>
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      int32_t first_val = INT32_MIN;
      int first_idx = -1;
      int32_t second_val = INT32_MIN;
      int second_idx = -1;
      const std::size_t t_offset = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        const int32_t l = logits[t_offset + static_cast<std::size_t>(e)];
        if (l > first_val) {
          second_val = first_val;
          second_idx = first_idx;
          first_val = l;
          first_idx = e;
        } else if (l > second_val) {
          second_val = l;
          second_idx = e;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(first_val)));
      hash = mix(hash, static_cast<uint64_t>(first_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }
  return hash;
}