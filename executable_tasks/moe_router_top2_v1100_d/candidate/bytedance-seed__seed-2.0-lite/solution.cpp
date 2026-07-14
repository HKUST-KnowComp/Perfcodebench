#include "interface.h"
#include <vector>
#include <climits>

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
      int32_t first_l = INT32_MIN;
      int first_idx = -1;
      int32_t second_l = INT32_MIN;
      int second_idx = -1;
      const size_t token_start = static_cast<size_t>(t) * static_cast<size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        const int32_t l = logits[token_start + static_cast<size_t>(e)];
        if (l > first_l || (l == first_l && e < first_idx)) {
          second_l = first_l;
          second_idx = first_idx;
          first_l = l;
          first_idx = e;
        } else if (l > second_l || (l == second_l && e < second_idx)) {
          second_l = l;
          second_idx = e;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(first_l)));
      hash = mix(hash, static_cast<uint64_t>(first_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_l)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }
  return hash;
}