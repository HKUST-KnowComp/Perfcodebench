#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  const size_t n_updates = indices.size();
  for (int iter = 0; iter < iters; ++iter) {
    // Apply all sparse updates
    for (size_t i = 0; i < n_updates; ++i) {
      accum[indices[i]] += static_cast<uint64_t>(values[i]);
    }
    // Compute checksum over full domain with sequential cache-friendly access
    hash = 1469598103934665603ULL;
    for (uint64_t val : accum) {
      hash = mix(hash, val);
    }
    // Reset only modified sparse entries for next iteration
    for (size_t i = 0; i < n_updates; ++i) {
      accum[indices[i]] = 0;
    }
  }
  return hash;
}