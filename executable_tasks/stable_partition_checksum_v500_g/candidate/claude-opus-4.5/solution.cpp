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

  // Compute prefix sums of flags to determine output positions
  // prefix[i] = number of true flags in flags[0..i-1]
  std::vector<std::size_t> prefix(n + 1);
  prefix[0] = 0;
  for (std::size_t i = 0; i < n; ++i) {
    prefix[i + 1] = prefix[i] + (flags[i] ? 1 : 0);
  }
  const std::size_t left_count = prefix[n];

  // Pre-allocate output buffer
  std::vector<uint32_t> out(n);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Scatter: place each element at its final position
    // Elements with flag=true go to positions [0, left_count)
    // Elements with flag=false go to positions [left_count, n)
    std::size_t right_idx = left_count;
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
