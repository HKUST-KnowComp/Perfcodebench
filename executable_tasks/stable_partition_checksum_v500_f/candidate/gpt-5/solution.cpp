#include "interface.h"

#include <vector>
#include <cstdint>

namespace {
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
  return (hash ^ value) * FNV_PRIME;
}
}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int /*iters*/) {
  const std::size_t n = values.size();
  uint64_t hash = FNV_OFFSET_BASIS;

  // First pass: values with flag true, in original order.
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      hash = mix(hash, static_cast<uint64_t>(values[i]));
    }
  }
  // Second pass: values with flag false, in original order.
  for (std::size_t i = 0; i < n; ++i) {
    if (!flags[i]) {
      hash = mix(hash, static_cast<uint64_t>(values[i]));
    }
  }

  return hash;
}
