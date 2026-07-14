#include "interface.h"

#include <cstring>
#include <vector>

namespace {

uint64_t checksum_vec(const std::vector<uint64_t>& agg, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    hash ^= agg[g];
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
    
    std::size_t i = 0;
    const std::size_t n_unroll = n - (n % 4);
    
    // Unrolled loop for better ILP
    for (; i < n_unroll; i += 4) {
      agg[keys[i]] += values[i];
      agg[keys[i+1]] += values[i+1];
      agg[keys[i+2]] += values[i+2];
      agg[keys[i+3]] += values[i+3];
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      agg[keys[i]] += values[i];
    }
    
    out = checksum_vec(agg, groups);
  }
  return out;
}
