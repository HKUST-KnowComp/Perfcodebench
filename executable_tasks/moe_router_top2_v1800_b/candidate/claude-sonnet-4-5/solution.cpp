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
  std::vector<std::pair<int32_t, int>> row(static_cast<std::size_t>(experts));
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const std::size_t offset = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        row[static_cast<std::size_t>(e)] = {logits[offset + static_cast<std::size_t>(e)], e};
      }
      
      std::nth_element(row.begin(), row.begin() + 1, row.end(), better);
      
      const auto& first = row[0];
      const auto& second = row[1];
      
      const auto& top1 = better(first, second) ? first : second;
      const auto& top2 = better(first, second) ? second : first;
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1.first)));
      hash = mix(hash, static_cast<uint64_t>(top1.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2.first)));
      hash = mix(hash, static_cast<uint64_t>(top2.second));
    }
  }
  return hash;
}