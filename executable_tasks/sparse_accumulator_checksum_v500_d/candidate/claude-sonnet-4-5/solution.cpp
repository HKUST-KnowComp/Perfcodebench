#include "interface.h"

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
    std::memset(accum.data(), 0, domain * sizeof(uint64_t));
    
    const std::size_t size = indices.size();
    for (std::size_t i = 0; i < size; ++i) {
      accum[indices[i]] += static_cast<uint64_t>(values[i]);
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, accum[key]);
    }
  }
  
  return hash;
}