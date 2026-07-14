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
  // Use a dense vector to avoid the overhead of std::unordered_map
  // Pre-allocate to avoid repeated allocations across iterations
  std::vector<uint64_t> accum(domain, 0);
  uint64_t hash = 0;

  const size_t n = indices.size();
  const uint32_t* idx_ptr = indices.data();
  const uint32_t* val_ptr = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset accumulator for this iteration
    std::fill(accum.begin(), accum.end(), 0);

    // Accumulate weighted updates
    for (size_t i = 0; i < n; ++i) {
      accum[idx_ptr[i]] += static_cast<uint64_t>(val_ptr[i]);
    }

    // Compute checksum over the bounded domain
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, accum[key]);
    }
  }

  return hash;
}