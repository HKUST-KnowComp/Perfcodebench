#include "interface.h"

#include <vector>

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
  const size_t n = values.size();
  if (n == 0) return 0;

  // Pre-allocate a buffer to avoid repeated allocations inside the loop
  std::vector<uint32_t> buffer(n);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    size_t left_idx = 0;
    size_t right_idx = 0;
    
    // We need to know where the 'right' partition starts to fill it linearly
    // First pass: count flags to find the split point
    size_t count_true = 0;
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) count_true++;
    }

    size_t write_left = 0;
    size_t write_right = count_true;

    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        buffer[write_left++] = values[i];
      } else {
        buffer[write_right++] = values[i];
      }
    }

    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      hash = mix(hash, buffer[i]);
    }
  }
  return hash;
}