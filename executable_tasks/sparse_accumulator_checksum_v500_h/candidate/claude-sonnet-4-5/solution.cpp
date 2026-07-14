#include "interface.h"

#include <vector>
#include <cstring>

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
  std::vector<uint64_t> accum(domain);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Fast zero using memset
    std::memset(accum.data(), 0, domain * sizeof(uint64_t));
    
    // Accumulate sparse updates
    const std::size_t n = indices.size();
    for (std::size_t i = 0; i < n; ++i) {
      accum[indices[i]] += static_cast<uint64_t>(values[i]);
    }
    
    // Compute hash over entire domain
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, accum[key]);
    }
  }
  
  return hash;
}
