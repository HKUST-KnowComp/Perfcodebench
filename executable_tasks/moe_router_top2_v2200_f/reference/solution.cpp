#include "interface.h"

#include <utility>

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
      std::pair<int32_t, int> best1{-2147483647 - 1, experts};
      std::pair<int32_t, int> best2{-2147483647 - 1, experts};
      const std::size_t row = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        const std::pair<int32_t, int> cur{logits[row + static_cast<std::size_t>(e)], e};
        if (better(cur, best1)) {
          best2 = best1;
          best1 = cur;
        } else if (better(cur, best2)) {
          best2 = cur;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1.first)));
      hash = mix(hash, static_cast<uint64_t>(best1.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2.first)));
      hash = mix(hash, static_cast<uint64_t>(best2.second));
    }
  }
  return hash;
}
