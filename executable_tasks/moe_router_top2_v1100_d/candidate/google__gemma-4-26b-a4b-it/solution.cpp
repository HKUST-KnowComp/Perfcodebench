#include "interface.h"

#include <cstdint>
#include <vector>
#include <limits>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct Candidate {
  int32_t val;
  int idx;

  // Tie-breaking: higher value first, then lower index first.
  // We want to keep the 'best' two, so we define 'better' to identify the top elements.
  inline bool is_better_than(const Candidate& other) const {
    if (val != other.val) return val > other.val;
    return idx < other.idx;
  }
};

} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* logits_ptr = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_start = logits_ptr + (static_cast<size_t>(t) * experts);
      
      // Initialize top 2 with values that are 'worse' than any possible input
      // Using min value for logit and max value for index to satisfy tie-break logic
      Candidate top1 = {std::numeric_limits<int32_t>::min(), std::numeric_limits<int>::max()};
      Candidate top2 = {std::numeric_limits<int32_t>::min(), std::numeric_limits<int>::max()};

      for (int e = 0; e < experts; ++e) {
        Candidate current = {row_start[e], e};
        
        if (current.is_better_than(top1)) {
          top2 = top1;
          top1 = current;
        } else if (current.is_better_than(top2)) {
          top2 = current;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1.val)));
      hash = mix(hash, static_cast<uint64_t>(top1.idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2.val)));
      hash = mix(hash, static_cast<uint64_t>(top2.idx));
    }
  }
  return hash;
}
