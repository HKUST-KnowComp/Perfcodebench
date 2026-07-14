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
      int32_t logit0 = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + 0];
      int32_t logit1 = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + 1];
      std::pair<int32_t, int> first = {logit0, 0};
      std::pair<int32_t, int> second = {logit1, 1};

      if (better(second, first)) {
        std::swap(first, second);
      }

      for (int e = 2; e < experts; ++e) {
        int32_t logit = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + e];
        std::pair<int32_t, int> current = {logit, e};
        if (better(current, first)) {
          second = first;
          first = current;
        } else if (better(current, second)) {
          second = current;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(first.first)));
      hash = mix(hash, static_cast<uint64_t>(first.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second.first)));
      hash = mix(hash, static_cast<uint64_t>(second.second));
    }
  }
  return hash;
}