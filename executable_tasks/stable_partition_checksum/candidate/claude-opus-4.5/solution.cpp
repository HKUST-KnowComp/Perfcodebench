#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  if (n == 0) {
    uint64_t hash = 1469598103934665603ULL;
    return hash;
  }

  // Pre-allocate output buffer once
  std::vector<uint32_t> out(n);

  // Count how many go to the left partition
  std::size_t left_count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    left_count += flags[i] ? 1 : 0;
  }

  const uint8_t* __restrict f = flags.data();
  const uint32_t* __restrict v = values.data();
  uint32_t* __restrict o = out.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Two-pointer scatter: left_idx starts at 0, right_idx starts at left_count
    std::size_t left_idx = 0;
    std::size_t right_idx = left_count;

    // Scatter values into correct positions
    for (std::size_t i = 0; i < n; ++i) {
      if (f[i]) {
        o[left_idx++] = v[i];
      } else {
        o[right_idx++] = v[i];
      }
    }

    // Compute hash
    hash = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;

    // Process 4 elements at a time for better ILP
    std::size_t i = 0;
    std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      hash ^= o[i];
      hash *= FNV_PRIME;
      hash ^= o[i + 1];
      hash *= FNV_PRIME;
      hash ^= o[i + 2];
      hash *= FNV_PRIME;
      hash ^= o[i + 3];
      hash *= FNV_PRIME;
    }
    for (; i < n; ++i) {
      hash ^= o[i];
      hash *= FNV_PRIME;
    }
  }

  return hash;
}