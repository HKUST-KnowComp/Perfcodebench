#include "interface.h"
#include <cstdint>
#include <vector>

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
  // Use dense accumulator with timestamp array to avoid clearing entire domain each iteration.
  std::vector<uint64_t> accum(domain, 0);
  std::vector<int64_t> timestamp(domain, 0);
  int64_t current = 0;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    ++current;
    // Accumulate sparse updates
    for (std::size_t i = 0; i < indices.size(); ++i) {
      uint32_t idx = indices[i];
      if (timestamp[idx] != current) {
        accum[idx] = static_cast<uint64_t>(values[i]);
        timestamp[idx] = current;
      } else {
        accum[idx] += static_cast<uint64_t>(values[i]);
      }
    }
    // Compute hash over full domain
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      uint64_t val = (timestamp[key] == current) ? accum[key] : 0ULL;
      hash = mix(hash, val);
    }
  }
  return hash;
}
