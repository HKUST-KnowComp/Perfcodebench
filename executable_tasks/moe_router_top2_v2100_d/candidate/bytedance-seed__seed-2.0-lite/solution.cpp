#include "interface.h"
#include <cstdint>
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
  uint64_t last_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    const std::size_t experts_sz = static_cast<std::size_t>(experts);
    for (int t = 0; t < tokens; ++t) {
      const std::size_t base = static_cast<std::size_t>(t) * experts_sz;
      int32_t top1_val = INT32_MIN;
      int top1_idx = -1;
      int32_t top2_val = INT32_MIN;
      int top2_idx = -1;
      for (int e = 0; e < experts; ++e) {
        const std::size_t pos = base + static_cast<std::size_t>(e);
        const int32_t cur_val = logits[pos];
        const int cur_idx = e;
        if (cur_val > top1_val) {
          top2_val = top1_val;
          top2_idx = top1_idx;
          top1_val = cur_val;
          top1_idx = cur_idx;
        } else if (cur_val > top2_val) {
          top2_val = cur_val;
          top2_idx = cur_idx;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_val)));
      hash = mix(hash, static_cast<uint64_t>(top1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_val)));
      hash = mix(hash, static_cast<uint64_t>(top2_idx));
    }
    last_hash = hash;
  }
  return last_hash;
}