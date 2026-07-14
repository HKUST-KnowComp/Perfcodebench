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
  
  const std::size_t n = indices.size();
  const uint32_t* idx_ptr = indices.data();
  const uint32_t* val_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(accum.data(), 0, domain * sizeof(uint64_t));
    
    for (std::size_t i = 0; i < n; ++i) {
      accum[idx_ptr[i]] += static_cast<uint64_t>(val_ptr[i]);
    }
    
    hash = 1469598103934665603ULL;
    uint64_t* acc_ptr = accum.data();
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, acc_ptr[key]);
    }
  }
  
  return hash;
}