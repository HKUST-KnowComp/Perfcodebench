#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  if (n == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  // Precompute prefix sums for left partition indices
  // prefix[i] = number of flags[j] == 1 for j < i
  std::vector<uint32_t> prefix(n + 1);
  prefix[0] = 0;
  uint32_t count_left = 0;
  for (std::size_t i = 0; i < n; ++i) {
    prefix[i] = count_left;
    count_left += (flags[i] != 0) ? 1 : 0;
  }
  prefix[n] = count_left;

  // Allocate output buffer once
  std::vector<uint32_t> out(n);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Scatter values to their stable-partition positions
    // Left partition: indices 0..count_left-1
    // Right partition: indices count_left..n-1
    uint32_t right_idx = count_left;
    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        out[prefix[i]] = values[i];
      } else {
        out[right_idx++] = values[i];
      }
    }

    // Compute hash
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, out[i]);
    }
  }
  return hash;
}