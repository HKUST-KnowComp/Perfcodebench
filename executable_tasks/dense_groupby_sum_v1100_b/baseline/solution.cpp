#include "interface.h"

#include <unordered_map>
#include <vector>

namespace {

uint64_t checksum_map(const std::unordered_map<uint32_t, uint64_t>& agg, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    const auto it = agg.find(static_cast<uint32_t>(g));
    const uint64_t v = (it == agg.end()) ? 0 : it->second;
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  std::unordered_map<uint32_t, uint64_t> agg;
  uint64_t out = 0;
  for (int iter = 0; iter < iters; ++iter) {
    agg.clear();
    for (std::size_t i = 0; i < keys.size(); ++i) {
      agg[keys[i]] += static_cast<uint64_t>(values[i]);
    }
    out = checksum_map(agg, groups);
  }
  return out;
}
