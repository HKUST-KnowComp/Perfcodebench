#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0 || keys.empty()) return 0;

  // For bounded key spaces, a dense mark array is much faster than hashing.
  // Use generation stamping to avoid clearing the whole array every iteration.
  std::vector<uint32_t> marks(static_cast<size_t>(key_space), 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    // If generation wraps, reset marks once and continue.
    if (gen == 0) {
      for (uint32_t &m : marks) m = 0;
      gen = 1;
    }

    for (uint32_t key : keys) {
      // Task contract implies keys are within the bounded key space.
      uint32_t &slot = marks[key];
      if (slot != gen) {
        slot = gen;
        ++local;
      }
    }

    count = local;
    ++gen;
  }

  return count;
}
