#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0 || keys.empty()) return 0;

  // Dense bounded-key counting using generation stamps.
  // Avoids clearing O(key_space) memory every iteration.
  std::vector<uint32_t> marks(static_cast<size_t>(key_space), 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    // Fast path while generation value is non-zero.
    if (gen != 0) {
      for (uint32_t key : keys) {
        // Task contract implies bounded keys: key < key_space.
        if (marks[key] != gen) {
          marks[key] = gen;
          ++local;
        }
      }
      count = local;
      ++gen;
      continue;
    }

    // Rare wraparound path: reset marks and restart generation numbering.
    for (size_t i = 0, n = marks.size(); i < n; ++i) marks[i] = 0;
    gen = 1;
    for (uint32_t key : keys) {
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
