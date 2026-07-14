#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (keys.empty() || key_space == 0) return 0;

  // Dense bounded-key case: use a generation-stamped table to avoid
  // clearing key_space entries every iteration.
  std::vector<uint32_t> marks(static_cast<size_t>(key_space), 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    const uint32_t cur = gen;

    for (uint32_t key : keys) {
      // Task contract implies bounded keys; keep a guard for robustness.
      if (key < key_space && marks[key] != cur) {
        marks[key] = cur;
        ++local;
      }
    }

    count = local;

    ++gen;
    if (gen == 0) {
      // Extremely rare wraparound: reset marks and restart generations.
      for (size_t i = 0, n = marks.size(); i < n; ++i) marks[i] = 0;
      gen = 1;
    }
  }

  return count;
}
