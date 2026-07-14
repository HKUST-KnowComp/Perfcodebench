#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (keys.empty() || key_space == 0) return 0;

  // Fast path for bounded key space: use a generation-stamped table to avoid
  // clearing O(key_space) memory on every iteration.
  std::vector<uint32_t> marks(static_cast<size_t>(key_space), 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    // If generation wraps, reset the table once and restart generations.
    if (gen == 0) {
      for (size_t i = 0, n = marks.size(); i < n; ++i) marks[i] = 0;
      gen = 1;
    }

    for (size_t i = 0, n = keys.size(); i < n; ++i) {
      uint32_t k = keys[i];
      // Per task contract, keys are bounded by key_space.
      if (marks[k] != gen) {
        marks[k] = gen;
        ++local;
      }
    }

    count = local;
    ++gen;
  }

  return count;
}
