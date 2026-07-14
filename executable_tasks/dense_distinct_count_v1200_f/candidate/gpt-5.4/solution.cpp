#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (keys.empty() || key_space == 0) return 0;

  // Bounded key space allows a dense marking table.
  // Use generation stamping to avoid clearing the table every iteration.
  std::vector<uint32_t> marks(static_cast<size_t>(key_space), 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    // Extremely unlikely in benchmark settings, but handle wraparound correctly.
    if (gen == 0) {
      for (size_t i = 0, n = marks.size(); i < n; ++i) marks[i] = 0;
      gen = 1;
    }

    for (uint32_t key : keys) {
      // Per task contract, keys are within bounded key_space.
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
