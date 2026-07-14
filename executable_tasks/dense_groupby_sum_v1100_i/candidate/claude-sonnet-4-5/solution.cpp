#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t checksum_vec(const std::vector<uint64_t>& agg, int groups) {
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
  std::vector<uint64_t> agg(groups);
  uint64_t out = 0;
  const std::size_t n = keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(agg.data(), 0, groups * sizeof(uint64_t));
    
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t key = keys[i];
      const uint64_t val = static_cast<uint64_t>(values[i]);
      agg[key] += val;
    }
    
    out = checksum_vec(agg, groups);
  }
  return out;
}
