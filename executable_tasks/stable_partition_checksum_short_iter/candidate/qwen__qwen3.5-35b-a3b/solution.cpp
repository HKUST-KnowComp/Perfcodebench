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
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (!flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
  }
  return hash;
}