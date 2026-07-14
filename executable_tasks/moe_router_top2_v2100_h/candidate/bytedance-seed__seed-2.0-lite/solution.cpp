#include "interface.h"
#include <climits>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const size_t experts_sz = static_cast<size_t>(experts);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const size_t token_start = static_cast<size_t>(t) * experts_sz;
      int32_t max1_val = INT32_MIN;
      int max1_idx = -1;
      int32_t max2_val = INT32_MIN;
      int max2_idx = -1;

      for (int e = 0; e < experts; ++e) {
        const int32_t l = logits[token_start + e];
        if (l > max1_val) {
          max2_val = max1_val;
          max2_idx = max1_idx;
          max1_val = l;
          max1_idx = e;
        } else if (l > max2_val) {
          max2_val = l;
          max2_idx = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max1_val)));
      hash = mix(hash, static_cast<uint64_t>(max1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max2_val)));
      hash = mix(hash, static_cast<uint64_t>(max2_idx));
    }
    final_hash = hash;
  }
  return final_hash;
}