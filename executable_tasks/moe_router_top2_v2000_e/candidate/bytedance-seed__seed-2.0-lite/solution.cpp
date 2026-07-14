#include "interface.h"

#include <utility>
#include <vector>
#include <climits>

namespace {

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
      std::pair<int32_t, int> first{INT32_MIN, experts};
      std::pair<int32_t, int> second{INT32_MIN, experts};
      const std::size_t t_offset = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        const int32_t l = logits[t_offset + e];
        const bool better_than_first = (l > first.first) || (l == first.first && e < first.second);
        if (better_than_first) {
          second = first;
          first = {l, e};
        } else {
          const bool better_than_second = (l > second.first) || (l == second.first && e < second.second);
          if (better_than_second) {
            second = {l, e};
          }
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