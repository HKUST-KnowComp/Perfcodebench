#include "interface.h"

#include <algorithm>
#include <climits>
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
  const size_t num_experts = static_cast<size_t>(experts);
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const size_t base = static_cast<size_t>(t) * num_experts;
      std::pair<int32_t, int> first{INT32_MIN, -1};
      std::pair<int32_t, int> second{INT32_MIN, -1};
      for (int e = 0; e < experts; ++e) {
        const std::pair<int32_t, int> curr{logits[base + static_cast<size_t>(e)], e};
        if (better(curr, first)) {
          second = first;
          first = curr;
        } else if (better(curr, second)) {
          second = curr;
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