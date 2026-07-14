#pragma once

#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Use a bitmap (byte array) for O(1) lookups instead of unordered_set.
  // key_space defines the bound. We assume keys are within [0, key_space).
  // Allocate once outside the iteration loop to save overhead.
  std::vector<uint8_t> bitmap(key_space, 0);
  
  // Mark enabled keys
  for (uint32_t k : enabled_keys) {
    if (k < key_space) {
      bitmap[k] = 1;
    }
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      uint32_t k = probe_keys[i];
      // Check bounds to prevent OOB access and match reference behavior
      if (k < key_space && bitmap[k]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}