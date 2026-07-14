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
  const std::size_t n = values.size();
  std::vector<uint32_t> out;
  out.reserve(n);

  // Stable partition: true flags first, then false flags
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      out.push_back(values[i]);
    }
  }
  for (std::size_t i = 0; i < n; ++i) {
    if (!flags[i]) {
      out.push_back(values[i]);
    }
  }

  constexpr uint64_t seed = 1469598103934665603ULL;
  constexpr uint64_t multiplier = 1099511628211ULL;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = seed;
    for (std::size_t i = 0; i < n; ++i) {
      h ^= out[i];
      h *= multiplier;
    }
    hash = h;
  }
  return hash;
}