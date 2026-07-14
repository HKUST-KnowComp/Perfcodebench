#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Find top-2 experts for a single row of logits in O(n) time
// Tie-breaking: higher logit wins, lower expert index breaks ties
void find_top2(const int32_t* logits, int experts, 
               std::pair<int32_t, int>& top1, 
               std::pair<int32_t, int>& top2) {
  top1 = {-2147483648, -1};
  top2 = {-2147483648, -1};
  
  for (int e = 0; e < experts; ++e) {
    int32_t logit = logits[e];
    if (logit > top1.first || (logit == top1.first && e < top1.second)) {
      top2 = top1;
      top1 = {logit, e};
    } else if (logit > top2.first || (logit == top2.first && e < top2.second)) {
      top2 = {logit, e};
    }
  }
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      std::pair<int32_t, int> top1, top2;
      find_top2(&logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts)], 
                experts, top1, top2);
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1.first)));
      hash = mix(hash, static_cast<uint64_t>(top1.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2.first)));
      hash = mix(hash, static_cast<uint64_t>(top2.second));
    }
  }
  return hash;
}