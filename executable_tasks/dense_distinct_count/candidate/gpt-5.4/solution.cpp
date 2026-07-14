#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (keys.empty() || key_space == 0) return 0;

  // Dense bounded-key counting using a generation-stamped table.
  // This avoids clearing O(key_space) memory every iteration and replaces
  // hash-table work with a single indexed load/store per key.
  std::vector<uint32_t> marks(key_space, 0);
  uint32_t stamp = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    // Handle rare stamp wraparound by resetting the table once.
    if (stamp == 0) {
      for (uint32_t &v : marks) v = 0;
      stamp = 1;
    }

    for (uint32_t key : keys) {
      // Per task contract, keys are bounded by key_space.
      if (marks[key] != stamp) {
        marks[key] = stamp;
        ++local;
      }
    }

    count = local;
    ++stamp;
  }

  return count;
}
