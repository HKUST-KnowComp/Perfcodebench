#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (keys.empty() || key_space == 0) return 0;

  // Bounded key space allows a dense marking table, which is much faster than
  // hashing for repeated exact distinct counts.
  std::vector<uint32_t> marks(static_cast<size_t>(key_space), 0);
  uint32_t epoch = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    // If epoch wraps, clear the table once and restart epochs.
    if (epoch == 0) {
      for (uint32_t &m : marks) m = 0;
      epoch = 1;
    }

    for (uint32_t key : keys) {
      // Contract says keys are bounded by key_space.
      if (marks[key] != epoch) {
        marks[key] = epoch;
        ++local;
      }
    }

    count = local;
    ++epoch;
  }

  return count;
}
