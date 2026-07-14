#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t compute_checksum(const std::vector<uint64_t>& agg, int groups) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    uint64_t v = agg[g];
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  // Use a vector as a direct-address table for O(1) access and better cache locality
  std::vector<uint64_t> agg(groups, 0);
  uint64_t out = 0;
  
  const uint32_t* k_ptr = keys.data();
  const uint32_t* v_ptr = values.data();
  const size_t n = keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the accumulator for the current iteration
    std::fill(agg.begin(), agg.end(), 0);
    
    // Perform the aggregation
    for (size_t i = 0; i < n; ++i) {
      agg[k_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
    }
    
    // Calculate checksum for this iteration
    out = compute_checksum(agg, groups);
  }
  
  return out;
}