#include "interface.h"

#include <vector>

namespace {

uint64_t checksum_map(const std::vector<uint64_t>& agg, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    const uint64_t v = agg[g];
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  std::vector<uint64_t> agg(groups, 0);
  std::vector<uint32_t> used_groups;
  used_groups.reserve(groups);
  uint64_t out = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < keys.size(); ++i) {
      const uint32_t k = keys[i];
      if (agg[k] == 0) {
        used_groups.push_back(k);
      }
      agg[k] += static_cast<uint64_t>(values[i]);
    }
    out = checksum_map(agg, groups);
    for (const uint32_t k : used_groups) {
      agg[k] = 0;
    }
    used_groups.clear();
  }
  return out;
}