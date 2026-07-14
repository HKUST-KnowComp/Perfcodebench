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
  // Use a dense vector instead of a map for O(1) access and better cache locality
  std::vector<uint64_t> accum(domain, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset accumulator for each iteration
    std::fill(accum.begin(), accum.end(), 0);

    // Accumulate weighted updates
    const size_t n = indices.size();
    for (size_t i = 0; i < n; ++i) {
      accum[indices[i]] += static_cast<uint64_t>(values[i]);
    }

    // Compute checksum for the current state
    uint64_t current_hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      current_hash = mix(current_hash, accum[key]);
    }
    final_hash = current_hash;
  }

  return final_hash;
}