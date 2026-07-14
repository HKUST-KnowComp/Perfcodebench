#pragma once

#include <cstdint>
#include <vector>

#include "interface.h"

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  // Use a direct addressing table with a generation counter (cookie) to avoid
  // O(key_space) clearing cost per iteration. This is much faster than
  // std::unordered_set for bounded keys.
  std::vector<uint32_t> seen(key_space, 0);
  uint32_t cookie = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (uint32_t key : keys) {
      // Check if key has been seen in the current iteration
      if (seen[key] != cookie) {
        seen[key] = cookie;
        count++;
      }
    }
    // Increment cookie for the next iteration to invalidate previous marks
    cookie++;
  }

  return count;
}