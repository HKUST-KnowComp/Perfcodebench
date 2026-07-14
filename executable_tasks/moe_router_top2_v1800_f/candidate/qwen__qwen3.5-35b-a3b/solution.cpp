#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

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
      int32_t logit0 = logits[t * experts + 0];
      int32_t logit1 = logits[t * experts + 1];
      int e0 = 0;
      int e1 = 1;

      if (better({logit1, e1}, {logit0, e0})) {
        std::swap(e0, e1);
        std::swap(logit0, logit1);
      }

      for (int e = 2; e < experts; ++e) {
        int32_t logit = logits[t * experts + e];
        if (better({logit, e}, {logit0, e0})) {
          logit1 = logit0;
          e1 = e0;
          logit0 = logit;
          e0 = e;
        } else if (better({logit, e}, {logit1, e1})) {
          logit1 = logit;
          e1 = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(logit0)));
      hash = mix(hash, static_cast<uint64_t>(e0));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(logit1)));
      hash = mix(hash, static_cast<uint64_t>(e1));
    }
  }
  return hash;
}