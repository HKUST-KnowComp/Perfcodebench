#include "interface.h"

#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  // Compute partitioned output once
  std::vector<uint32_t> out(values.size());
  uint32_t* out_ptr = out.data();
  // First pass: true flags
  for (std::size_t i = 0; i < flags.size(); ++i) {
    if (flags[i]) {
      *out_ptr++ = values[i];
    }
  }
  // Second pass: false flags
  for (std::size_t i = 0; i < flags.size(); ++i) {
    if (!flags[i]) {
      *out_ptr++ = values[i];
    }
  }
  // Compute hash iters times
  uint64_t hash = 0;
  const uint32_t* data = out.data();
  std::size_t n = out.size();
  constexpr uint64_t fnv_offset = 1469598103934665603ULL;
  constexpr uint64_t fnv_prime = 1099511628211ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset;
    for (std::size_t i = 0; i < n; ++i) {
      hash ^= data[i];
      hash *= fnv_prime;
    }
  }
  return hash;
}
