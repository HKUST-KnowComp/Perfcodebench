#include "interface.h"

#include <vector>
#include <algorithm>

namespace {

uint64_t checksum_sums(const std::vector<uint64_t>& agg, int groups) {
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
  std::vector<uint64_t> sums(groups, 0);
  uint64_t out = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);
    for (std::size_t i = 0; i < keys.size(); ++i) {
      sums[keys[i]] += static_cast<uint64_t>(values[i]);
    }
    out = checksum_sums(sums, groups);
  }
  return out;
}