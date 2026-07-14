#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  (void)iters;  // unused; input is constant, result is identical each iteration

  std::size_t n = values.size();
  std::vector<uint32_t> out(n);

  // First pass: place true-flag elements preserving order
  std::size_t pos = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      out[pos++] = values[i];
    }
  }
  // Second pass: place false-flag elements preserving order
  for (std::size_t i = 0; i < n; ++i) {
    if (!flags[i]) {
      out[pos++] = values[i];
    }
  }

  // Compute FNV-1a hash of the stable-partitioned output
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
  for (uint32_t v : out) {
    hash ^= v;
    hash *= 1099511628211ULL;  // FNV prime
  }
  return hash;
}
