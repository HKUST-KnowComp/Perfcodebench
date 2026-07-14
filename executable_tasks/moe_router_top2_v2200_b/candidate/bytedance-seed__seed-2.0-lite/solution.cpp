#include "interface.h"

#include <algorithm>
#include <climits>
#include <utility>
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

inline bool is_better(std::pair<int32_t, int> a, std::pair<int32_t, int> b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      std::pair<int32_t, int> top1 = {INT32_MIN, -1};
      std::pair<int32_t, int> top2 = {INT32_MIN, -1};
      const size_t token_offset = static_cast<size_t>(t) * static_cast<size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        std::pair<int32_t, int> curr = {logits[token_offset + e], e};
        if (is_better(curr, top1)) {
          top2 = top1;
          top1 = curr;
        } else if (is_better(curr, top2)) {
          top2 = curr;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1.first)));
      hash = mix(hash, static_cast<uint64_t>(top1.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2.first)));
      hash = mix(hash, static_cast<uint64_t>(top2.second));
    }
    final_hash = hash;
  }
  return final_hash;
}