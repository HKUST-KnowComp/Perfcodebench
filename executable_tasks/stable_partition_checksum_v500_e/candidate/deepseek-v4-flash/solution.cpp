#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t hash = 0;
  const size_t n = values.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    // First pass: left (flags true)
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        hash ^= values[i];
        hash *= 1099511628211ULL;
      }
    }
    // Second pass: right (flags false)
    for (size_t i = 0; i < n; ++i) {
      if (!flags[i]) {
        hash ^= values[i];
        hash *= 1099511628211ULL;
      }
    }
  }
  return hash;
}
