#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (keys.empty() || key_space == 0) return 0;

  // Fast path for bounded keys: use a generation-stamped table to avoid
  // clearing O(key_space) memory every iteration.
  std::vector<uint32_t> marks(key_space, 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    // Handle rare generation wraparound by resetting the table once.
    if (gen == 0) {
      for (uint32_t &m : marks) m = 0;
      gen = 1;
    }

    for (uint32_t key : keys) {
      // Per task contract, keys are bounded by key_space.
      if (marks[key] != gen) {
        marks[key] = gen;
        ++local;
      }
    }

    count = local;
    ++gen;
  }

  return count;
}
