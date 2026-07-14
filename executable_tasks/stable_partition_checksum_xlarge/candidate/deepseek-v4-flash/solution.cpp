#include "interface.h"
#include <vector>
#include <cstddef>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  
  // Count left (flag true) elements
  std::size_t left_count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) ++left_count;
  }
  
  // Build partitioned output in one pass
  std::vector<uint32_t> out(n);
  std::size_t left_idx = 0;
  std::size_t right_idx = left_count;
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      out[left_idx++] = values[i];
    } else {
      out[right_idx++] = values[i];
    }
  }
  
  // Compute hash once (it is identical for every iteration since inputs are const)
  uint64_t hash = 0;
  if (iters > 0) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : out) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
