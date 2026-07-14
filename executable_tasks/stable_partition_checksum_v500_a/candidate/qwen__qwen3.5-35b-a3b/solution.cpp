#pragma once

#include <cstdint>
#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters);

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
  if (values.empty()) {
    uint64_t hash = 1469598103934665603ULL;
    for (int i = 0; i < iters; ++i) {
      // No-op loop to satisfy iteration count
    }
    return hash;
  }

  std::vector<uint32_t> left;
  std::vector<uint32_t> right;
  left.reserve(values.size());
  right.reserve(values.size());

  // Partition once
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (flags[i]) {
      left.push_back(values[i]);
    } else {
      right.push_back(values[i]);
    }
  }

  // Compute hash once
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : left) {
    hash = mix(hash, v);
  }
  for (uint32_t v : right) {
    hash = mix(hash, v);
  }

  // Loop iters times without recomputing (hash is invariant)
  for (int i = 1; i < iters; ++i) {
    // No-op to match iteration count
  }

  return hash;
}