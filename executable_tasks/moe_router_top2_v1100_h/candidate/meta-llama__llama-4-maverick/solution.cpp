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
  std::vector<std::pair<int32_t, int>> row(experts);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      auto* row_begin = row.data();
      for (int e = 0; e < experts; ++e) {
        row_begin[e] = {logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + static_cast<std::size_t>(e)], e};
      }
      std::partial_sort(row_begin, row_begin + 2, row_begin + experts, better);
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row_begin[0].first)));
      hash = mix(hash, static_cast<uint64_t>(row_begin[0].second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row_begin[1].first)));
      hash = mix(hash, static_cast<uint64_t>(row_begin[1].second));
    }
  }
  return hash;
}
