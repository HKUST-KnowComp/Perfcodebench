#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct Candidate {
  int32_t val;
  int idx;
};

// Tie-breaking: higher value first, then lower index.
inline bool is_better(int32_t v1, int i1, int32_t v2, int i2) {
  if (v1 != v2) return v1 > v2;
  return i1 < i2;
}

} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_ptr = data + (static_cast<size_t>(t) * experts);
      
      // We only need the top 2 experts.
      // Initialize with values that are 'worse' than any possible input.
      // Using a very small value and a very large index for tie-breaking.
      Candidate top1 = {-2147483648, experts + 1};
      Candidate top2 = {-2147483648, experts + 1};

      for (int e = 0; e < experts; ++e) {
        int32_t current_val = row_ptr[e];
        int current_idx = e;

        if (is_better(current_val, current_idx, top1.val, top1.idx)) {
          top2 = top1;
          top1 = {current_val, current_idx};
        } else if (is_better(current_val, current_idx, top2.val, top2.idx)) {
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
