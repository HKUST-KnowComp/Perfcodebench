#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (keys.empty() || key_space == 0) return 0;

  // Dense bounded-key counting using generation stamps.
  // This avoids clearing O(key_space) memory every iteration and avoids the
  // heavy overhead of hashing in unordered_set.
  std::vector<uint32_t> seen(key_space, 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    const uint32_t cur = gen;

    for (uint32_t key : keys) {
      // Per task contract, keys are bounded by key_space.
      if (seen[key] != cur) {
        seen[key] = cur;
        ++local;
      }
    }

    count = local;

    // Advance generation; handle wraparound safely.
    ++gen;
    if (gen == 0) {
      for (uint32_t &v : seen) v = 0;
      gen = 1;
    }
  }

  return count;
}
