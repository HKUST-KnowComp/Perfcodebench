#pragma once

#include <cstdint>
#include <vector>

#include "interface.h"

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Use a direct-address bitmap for O(1) lookup instead of O(1) average with high constant overhead
  // Allocate once outside the iteration loop to avoid redundant setup
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
      uint32_t key = probe_keys[i];
      if (key < key_space && bitmap[key]) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}