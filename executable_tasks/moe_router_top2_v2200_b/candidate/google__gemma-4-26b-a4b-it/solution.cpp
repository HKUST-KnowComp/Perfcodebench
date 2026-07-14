#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct Expert {
  int32_t logit;
  int index;
};

} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* logits_ptr = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_ptr = logits_ptr + (static_cast<size_t>(t) * experts);
      
      // We need the top 2 experts based on: 
      // 1. Higher logit
      // 2. Lower index (tie-breaker)
      Expert top1 = {-2147483648, -1};
      Expert top2 = {-2147483648, -1};

      for (int e = 0; e < experts; ++e) {
        int32_t val = row_ptr[e];
        // Comparison logic: better(a, b) is a.logit > b.logit || (a.logit == b.logit && a.index < b.index)
        // Check if current expert is better than top1
        if (val > top1.logit || (val == top1.logit && e < top1.index)) {
          top2 = top1;
          top1 = {val, e};
        } else if (val > top2.logit || (val == top2.logit && e < top2.index)) {
          top2 = {val, e};
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1.logit)));
      hash = mix(hash, static_cast<uint64_t>(top1.index));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2.logit)));
      hash = mix(hash, static_cast<uint64_t>(top2.index));
    }
  }
  return hash;
}
