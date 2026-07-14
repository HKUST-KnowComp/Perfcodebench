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
      const size_t base = static_cast<size_t>(t) * static_cast<size_t>(experts);
      // Initialize top-1 and top-2 with first two experts
      std::pair<int32_t, int> top1 = {logits[base + 0], 0};
      std::pair<int32_t, int> curr = {logits[base + 1], 1};
      if (better(curr, top1)) {
        std::swap(top1, curr);
      }
      std::pair<int32_t, int> top2 = curr;
      // Scan remaining experts to update top-2
      for (int e = 2; e < experts; ++e) {
        curr = {logits[base + e], e};
        if (better(curr, top1)) {
          top2 = top1;
          top1 = curr;
        } else if (better(curr, top2)) {
          top2 = curr;
        }
      }
      // Update checksum identically to original implementation
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1.first)));
      hash = mix(hash, static_cast<uint64_t>(top1.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2.first)));
      hash = mix(hash, static_cast<uint64_t>(top2.second));
    }
  }
  return hash;
}