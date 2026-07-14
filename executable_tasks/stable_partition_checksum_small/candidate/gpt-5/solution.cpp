#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL; // FNV offset basis
  const std::size_t n = values.size();

  // First pass: elements with flag != 0 (stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      hash ^= static_cast<uint64_t>(values[i]);
      hash *= 1099511628211ULL; // FNV prime
    }
  }
  // Second pass: elements with flag == 0 (stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (!flags[i]) {
      hash ^= static_cast<uint64_t>(values[i]);
      hash *= 1099511628211ULL;
    }
  }

  return hash;
}
