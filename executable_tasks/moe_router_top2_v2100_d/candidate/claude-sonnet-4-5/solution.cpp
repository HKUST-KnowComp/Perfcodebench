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
  std::vector<std::pair<int32_t, int>> row;
  row.resize(static_cast<std::size_t>(experts));
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      for (int e = 0; e < experts; ++e) {
        row[e] = {logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + static_cast<std::size_t>(e)], e};
      }
      std::nth_element(row.begin(), row.begin() + 1, row.end(), better);
      
      auto& first = row[0];
      auto& second = row[1];
      if (!better(first, second)) {
        std::swap(first, second);
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(first.first)));
      hash = mix(hash, static_cast<uint64_t>(first.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second.first)));
      hash = mix(hash, static_cast<uint64_t>(second.second));
    }
  }
  return hash;
}