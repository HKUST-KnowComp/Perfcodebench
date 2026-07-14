#include "interface.h"
#include <vector>
#include <cstdint>

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

// Tie-breaking: higher logit, then lower index
inline bool is_better(int32_t l1, int i1, int32_t l2, int i2) {
  if (l1 != l2) return l1 > l2;
  return i1 < i2;
}

} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* logits_ptr = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_ptr = logits_ptr + (static_cast<size_t>(t) * experts);
      
      // Initialize top 2 with values that are 'worse' than any possible input
      // Using min value for logit and max value for index to satisfy is_better
      Expert top1 = {-2147483648, 2147483647};
      Expert top2 = {-2147483648, 2147483647};

      for (int e = 0; e < experts; ++e) {
        int32_t current_logit = row_ptr[e];
        int current_idx = e;

        if (is_better(current_logit, current_idx, top1.logit, top1.index)) {
          top2 = top1;
          top1 = {current_logit, current_idx};
        } else if (is_better(current_logit, current_idx, top2.logit, top2.index)) {
          top2 = {current_logit, current_idx};
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
