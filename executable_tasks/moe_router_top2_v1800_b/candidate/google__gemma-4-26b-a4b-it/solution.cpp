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

  // Tie-breaking: higher value, then lower index
  inline bool is_better_than(int32_t other_val, int other_idx) const {
    if (val != other_val) return val > other_val;
    return idx < other_idx;
  }

  inline bool is_better_than(const Candidate& other) const {
    if (val != other.val) return val > other.val;
    return idx < other.idx;
  }
};

} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_ptr = data + (static_cast<size_t>(t) * experts);
      
      // Initialize top 2 with values that are 'worse' than any possible input
      // Using a very small value and a very large index to satisfy tie-breaking
      Candidate top1 = {std::numeric_limits<int32_t>::min(), experts};
      Candidate top2 = {std::numeric_limits<int32_t>::min(), experts};

      for (int e = 0; e < experts; ++e) {
        int32_t current_val = row_ptr[e];
        int current_idx = e;

        // Check if current is better than top1
        if (current_val > top1.val || (current_val == top1.val && current_idx < top1.idx)) {
          top2 = top1;
          top1 = {current_val, current_idx};
        } 
        // Else check if current is better than top2
        else if (current_val > top2.val || (current_val == top2.val && current_idx < top2.idx)) {
          top2 = {current_val, current_idx};
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
