#pragma once

#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  // Build O(1) lookup table from cache keys
  std::vector<bool> cache_lookup(hash_space, false);
  for (uint32_t key : sorted_cache_keys) {
    if (key < hash_space) {
      cache_lookup[key] = true;
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      for (int i = request_offsets[r]; i < request_offsets[r + 1]; ++i) {
        uint32_t block = request_blocks[static_cast<std::size_t>(i)];
        if (block >= hash_space || !cache_lookup[block]) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}