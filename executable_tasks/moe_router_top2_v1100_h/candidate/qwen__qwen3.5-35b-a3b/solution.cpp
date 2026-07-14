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
      // Initialize top two experts
      int32_t best_logit = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + 0];
      int best_idx = 0;
      int32_t second_logit = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + 1];
      int second_idx = 1;

      // Order the first two experts
      if (second_logit > best_logit || (second_logit == best_logit && second_idx < best_idx)) {
        std::swap(best_logit, second_logit);
        std::swap(best_idx, second_idx);
      }

      // Iterate over remaining experts
      for (int e = 2; e < experts; ++e) {
        int32_t logit = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + e];
        int idx = e;

        if (logit > best_logit || (logit == best_logit && idx < best_idx)) {
          second_logit = best_logit;
          second_idx = best_idx;
          best_logit = logit;
          best_idx = idx;
        } else if (logit > second_logit || (logit == second_logit && idx < second_idx)) {
          second_logit = logit;
          second_idx = idx;
        }
      }

      // Mix the top two experts into the hash
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_logit)));
      hash = mix(hash, static_cast<uint64_t>(best_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_logit)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }
  return hash;
}