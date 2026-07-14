#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  
  std::vector<uint64_t> accum(domain, 0);
  uint64_t hash = 0;
  const size_t n = indices.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset accumulator
    std::fill(accum.begin(), accum.end(), 0ULL);
    
    // Accumulate values
    for (size_t i = 0; i < n; ++i) {
      accum[indices[i]] += static_cast<uint64_t>(values[i]);
    }
    
    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, accum[key]);
    }
  }
  
  return hash;
}