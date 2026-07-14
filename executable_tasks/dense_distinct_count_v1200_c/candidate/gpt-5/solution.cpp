#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0) return 0;

  // Visit-stamp array: marks[key] == current_stamp iff key seen in current iteration
  std::vector<uint32_t> marks(key_space, 0u);
  uint32_t current_stamp = 1u;

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t c = 0;
    const uint32_t* kdata = keys.data();
    const size_t n = keys.size();
    uint32_t* mdata = marks.data();

    // Assumes keys are in [0, key_space). This is the task's bounded-key guarantee.
    for (size_t i = 0; i < n; ++i) {
      uint32_t k = kdata[i];
      // Mark and count on first visit in this iteration
      if (mdata[k] != current_stamp) {
        mdata[k] = current_stamp;
        ++c;
      }
    }

    count = c;

    // Advance stamp; if wraps, clear and restart stamps to avoid ambiguity
    ++current_stamp;
    if (current_stamp == 0u) {
      std::fill(marks.begin(), marks.end(), 0u);
      current_stamp = 1u;
    }
  }

  return count;
}
