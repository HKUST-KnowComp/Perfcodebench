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
      std::pair<int32_t, int> top1 = {
          logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + 0], 0};
      std::pair<int32_t, int> top2 = {
          logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + 1], 1};
      if (better(top2, top1)) {
        std::swap(top1, top2);
      }
      for (int e = 2; e < experts; ++e) {
        std::pair<int32_t, int> current = {
            logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + e], e};
        if (better(current, top1)) {
          top2 = top1;
          top1 = current;
        } else if (better(current, top2)) {
          top2 = current;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1.first)));
      hash = mix(hash, static_cast<uint64_t>(top1.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2.first)));
      hash = mix(hash, static_cast<uint64_t>(top2.second));
    }
  }
  return hash;
}