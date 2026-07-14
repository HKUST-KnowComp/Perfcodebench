#include "interface.h"

#include <algorithm>
#include <vector>

namespace {

uint64_t checksum_u64(const std::vector<uint64_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : data) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  std::vector<uint64_t> agg(static_cast<std::size_t>(groups), 0);
  uint64_t out = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(agg.begin(), agg.end(), 0);
    for (std::size_t i = 0; i < keys.size(); ++i) {
      agg[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
    }
    out = checksum_u64(agg);
  }
  return out;
}
