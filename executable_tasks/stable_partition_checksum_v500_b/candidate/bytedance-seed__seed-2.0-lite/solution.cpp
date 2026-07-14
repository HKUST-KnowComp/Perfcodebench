#include "interface.h"

#include <vector>

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
  uint64_t hash = 0;
  const std::size_t n = values.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    // Add all true-flag values first to match stable partition left order
    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
    // Add all false-flag values next to match stable partition right order
    for (std::size_t i = 0; i < n; ++i) {
      if (!flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
  }
  return hash;
}