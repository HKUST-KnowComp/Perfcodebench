#include "interface.h"
#include <cstdint>
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
  const uint64_t init = 1469598103934665603ULL;
  const std::size_t n = values.size();
  uint64_t hash = init;

  for (int iter = 0; iter < iters; ++iter) {
    hash = init;
    // stable partition: all true elements first, in original order
    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
    for (std::size_t i = 0; i < n; ++i) {
      if (!flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
  }
  return hash;
}