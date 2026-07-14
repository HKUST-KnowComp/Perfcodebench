#pragma once

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
  (void)iters; // Unused after optimization
  std::vector<uint32_t> out;
  out.reserve(values.size());
  
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (flags[i]) {
      out.push_back(values[i]);
    }
  }
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (!flags[i]) {
      out.push_back(values[i]);
    }
  }
  
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : out) {
    hash = mix(hash, v);
  }
  return hash;
}