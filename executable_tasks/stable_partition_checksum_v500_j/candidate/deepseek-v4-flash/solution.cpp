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
  uint64_t hash = 0;
  const auto* flag_data = flags.data();
  const auto* value_data = values.data();
  std::size_t n = values.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    // Process flag == 1 (left)
    for (std::size_t i = 0; i < n; ++i) {
      if (flag_data[i]) {
        hash = mix(hash, value_data[i]);
      }
    }
    // Process flag == 0 (right)
    for (std::size_t i = 0; i < n; ++i) {
      if (!flag_data[i]) {
        hash = mix(hash, value_data[i]);
      }
    }
  }
  return hash;
}